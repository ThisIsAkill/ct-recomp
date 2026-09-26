#!/usr/bin/env python3
"""Verify funcs.toml against ChronoRET source and the ROM.

Each ChronoRET 'org' block is swept linearly through the ROM. Instruction
widths come from the decoder's state wherever a funcs.toml routine reaches
that address, otherwise from the propagated sweep state. Checks:
  - every decoded instruction inside a block starts on a source
    instruction with the same mnemonic
  - every funcs.toml label resolves to its funcs.toml address
  - blocks with a '(N bytes, $XXXX-$YYYY)' header sweep to exactly N bytes
Also cross-checks label addresses against a second disassembly when present.

Env: CHRONORET (default ../ChronoRET), CT_DISASM (default ../ct_disassembly).
"""
from __future__ import annotations

import os
import re
import sys

GAME = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))   # game/ct
REPO = os.path.dirname(os.path.dirname(GAME))
FUNCS_TOML = os.path.join(GAME, 'funcs.toml')
sys.path.insert(0, os.path.join(REPO, 'recomp'))

import decode
import game as game_cfg  # noqa: E402
import funcs  # noqa: E402
GAME_CFG = game_cfg.load(GAME)

BLOCK_RE = re.compile(r'^;.*\((\d+) bytes, \$([0-9A-F]{4})[–-]\$([0-9A-F]{4})\)')
# Header continuation: ';   Name (N byte(s), $XXXX...)' after a header ending in '+'.
MORE_RE = re.compile(r'^;\s+\S+ \((\d+) bytes?, \$[0-9A-F]{4}')
ORG_RE = re.compile(r'^org \$([0-9A-F]{6})', re.I)
LABEL_RE = re.compile(r'^([A-Za-z_][A-Za-z0-9_]*):')
INSN_RE = re.compile(r'^\s+([A-Za-z]{3})(?:\.[lwb])?\b')
DB_RE = re.compile(r'^\s+db\s+\$([0-9A-F]{2})', re.I)


class Block:
    def __init__(self, org: int, size: int | None):
        self.org, self.size = org, size
        self.mnemonics: list[str] = []
        self.labels: dict[str, int] = {}   # label -> source insn index
        self.addrs: list[int] = []          # swept address per source insn
        self.end: int | None = None         # first address past the sweep


def parse_chronoret(path: str, bank: int) -> list[Block]:
    blocks: list[Block] = []
    header = None
    cont = False
    cur: Block | None = None
    with open(path) as f:
        for line in f:
            line = line.rstrip('\n')
            m = BLOCK_RE.match(line)
            if m:
                header = (int(m.group(1)), int(m.group(2), 16))
                cont = line.rstrip().endswith('+')
                continue
            m = MORE_RE.match(line)
            if m and header and cont:
                header = (header[0] + int(m.group(1)), header[1])
                cont = line.rstrip().endswith('+')
                continue
            cont = False
            m = ORG_RE.match(line)
            if m:
                org = int(m.group(1), 16)
                size = header[0] if header and (org & 0xFFFF) == header[1] else None
                header = None
                cur = Block(org, size) if (org >> 16) == bank else None
                if cur:
                    blocks.append(cur)
                continue
            if cur is None:
                continue
            code = line.split(';', 1)[0]
            m = LABEL_RE.match(code)
            if m:
                cur.labels[m.group(1)] = len(cur.mnemonics)
                code = ' ' + code[m.end():]
            m = DB_RE.match(code)
            if m:
                cur.mnemonics.append(decode.OPCODES[int(m.group(1), 16)][0])
                continue
            m = INSN_RE.match(code)
            if m:
                cur.mnemonics.append(m.group(1).upper())
    return [b for b in blocks if b.mnemonics]


def parse_second(path: str) -> dict[str, int]:
    out: dict[str, int] = {}
    addr_re = re.compile(r';([0-9A-F]{6})\|')
    label = None
    with open(path) as f:
        for line in f:
            m = re.match(r'^\s*([A-Za-z_][A-Za-z0-9_]*):', line)
            if m:
                label = m.group(1)
            if label:
                a = addr_re.search(line)
                if a:
                    out.setdefault(label, int(a.group(1), 16))
                    label = None
    return out


def sweep(rom: bytes, blk: Block, states: dict[int, set], errors: list, notes: list) -> None:
    st = None
    addr = blk.org
    for k, src in enumerate(blk.mnemonics):
        known = states.get(addr)
        if known:
            if st is not None and (st.m, st.x) not in known and len(known) == 1:
                pass  # the routine's own state wins
            (m, x), *_ = sorted(known, key=lambda s: (s[0] is None, s))
            st = decode.State(m, x)
            sizes = {decode.decode_insn(rom, addr, decode.State(mm, xx)).size for mm, xx in known}
            if len(sizes) != 1:
                errors.append(f'${addr:06X}: variants decode to different sizes {sorted(sizes)}')
        if st is None:
            st = GAME_CFG.default_state(addr >> 16)
        try:
            i = decode.decode_insn(rom, addr, st)
        except decode.DecodeError as ex:
            notes.append(f'block ${blk.org:06X}: sweep stopped at ${addr:06X} ({ex})')
            return
        blk.addrs.append(addr)
        if i.mnemonic != src:
            msg = f'${addr:06X}: ROM decodes {i.mnemonic}, ChronoRET has {src}'
            (errors if known else notes).append(msg)
            if not known:
                return
        try:
            st = decode.next_state(i, st)
        except decode.DecodeError:
            st = decode.State(None, None)
        if i.mnemonic in decode.RETURNS or i.mnemonic in ('JMP', 'JML', 'BRA', 'BRL'):
            st = None  # unknown until a decoded routine says otherwise
        addr = i.next_addr
    blk.end = addr


def main() -> int:
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    cret = os.environ.get('CHRONORET', os.path.join(here, '..', 'ChronoRET'))
    second = os.environ.get('CT_DISASM', os.path.join(here, '..', 'ct_disassembly'))
    rom = decode.load_rom()
    metas = funcs.load(FUNCS_TOML)
    reg = funcs.Registry(rom, metas, FUNCS_TOML)
    errors: list[str] = []
    notes: list[str] = []

    states: dict[int, set] = {}
    decoded: dict[str, list] = {}
    for fm in metas:
        for st in fm.entry_states():
            fn = reg.function(fm.addr, st)
            decoded.setdefault(fm.name, []).append((st, fn))
            for i in fn.insns:
                states.setdefault(i.addr, set()).add((i.m, i.x))

    banks = sorted({fm.addr >> 16 for fm in metas})
    blocks: dict[int, list[Block]] = {}
    for bank in banks:
        path = os.path.join(cret, 'asm', f'bank{bank:02X}', f'bank{bank:02X}.asm')
        blocks[bank] = parse_chronoret(path, bank) if os.path.isfile(path) else []
        for blk in blocks[bank]:
            sweep(rom, blk, states, errors, notes)
            if blk.size is not None and blk.end is not None and blk.end - blk.org != blk.size:
                errors.append(f'block ${blk.org:06X}: sweeps {blk.end - blk.org} bytes, header says {blk.size}')

    starts = {a for bank in blocks for b in blocks[bank] for a in b.addrs}
    ranges = [(b.org, b.end if b.end is not None else (b.addrs[-1] + 1 if b.addrs else b.org))
              for bank in blocks for b in blocks[bank]]

    print(f'{"name":<40} {"addr":>7} {"states":<10} {"bytes":>5}  result')
    unverified = 0
    for fm in metas:
        res = []
        blk = next((b for b in blocks[fm.addr >> 16] if fm.name in b.labels), None)
        if blk is None:
            unverified += 1
        else:
            k = blk.labels[fm.name]
            if k >= len(blk.addrs):
                res.append('label past verified sweep')
            elif blk.addrs[k] != fm.addr:
                res.append(f'label at ${blk.addrs[k]:06X}, funcs.toml ${fm.addr:06X}')
        sizes = []
        for st, fn in decoded[fm.name]:
            sizes.append(fn.size)
            for i in fn.insns:
                inside = any(lo <= i.addr < hi for lo, hi in ranges)
                if inside and i.addr not in starts:
                    res.append(f'{st.tag()}: ${i.addr:06X} not on a ChronoRET instruction')
                    break
        verdict = 'FAIL' if res else ('ok' if blk is not None else 'not in ChronoRET')
        print(f'{fm.name:<40} ${fm.addr:06X} {",".join(fm.states):<10} '
              f'{"/".join(map(str, sorted(set(sizes)))):>5}  {verdict}')
        errors += [f'{fm.name}: {r}' for r in res]

    nblk = sum(len(v) for v in blocks.values())
    full = sum(1 for v in blocks.values() for b in v if b.end is not None)
    print(f'ChronoRET blocks: {nblk}, fully swept: {full}; routines outside ChronoRET: {unverified}')

    for bank in banks:
        path = os.path.join(second, f'bank_{bank:02X}.asm')
        if not os.path.isfile(path):
            print(f'cross-check: {path} not found, skipped')
            continue
        labels = parse_second(path)
        hit = 0
        for fm in metas:
            if fm.addr >> 16 != bank or fm.name not in labels:
                continue
            hit += 1
            if labels[fm.name] != fm.addr:
                errors.append(f'{fm.name}: second disassembly has ${labels[fm.name]:06X}')
        print(f'cross-check bank ${bank:02X}: {hit} labels compared')

    for n in notes:
        print(f'note: {n}')
    for e in errors:
        print(f'error: {e}', file=sys.stderr)
    return 1 if errors else 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except decode.DecodeError as ex:
        print(f'check_meta: {ex}', file=sys.stderr)
        sys.exit(1)
