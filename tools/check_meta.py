#!/usr/bin/env python3
"""Verify funcs.toml against ChronoRET source and the ROM.

For each ChronoRET block containing a funcs.toml label:
  - linear-decode the block's bytes; mnemonics must match the source and the
    decode must end exactly at the block's stated end and byte count
  - the label's address (from the sweep) must equal funcs.toml addr
  - each entry state's decoded function must match the source mnemonics
    from the label to its return, and the union of decoded bytes of the
    block's functions must equal the block's matched byte count
Optional: cross-check addresses against a second disassembly (label -> addr).

Env: CHRONORET (default ../ChronoRET), CT_DISASM (default ../ct_disassembly).
"""
from __future__ import annotations

import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'recomp'))

import decode  # noqa: E402
import funcs  # noqa: E402

BLOCK_RE = re.compile(r'^;.*\((\d+) bytes, \$([0-9A-F]{4})[–-]\$([0-9A-F]{4})\)')
ORG_RE = re.compile(r'^org \$([0-9A-F]{6})', re.I)
LABEL_RE = re.compile(r'^([A-Za-z_][A-Za-z0-9_]*):')
INSN_RE = re.compile(r'^\s+([A-Za-z]{3})(?:\.[lwb])?\b')


class Block:
    def __init__(self, size: int, start: int, end: int, org: int):
        self.size, self.start, self.end, self.org = size, start, end, org
        self.mnemonics: list[str] = []
        self.labels: dict[str, int] = {}   # label -> insn index


def parse_chronoret(path: str, bank: int) -> list[Block]:
    blocks: list[Block] = []
    pending = None
    cur: Block | None = None
    with open(path) as f:
        for line in f:
            line = line.rstrip('\n')
            m = BLOCK_RE.match(line)
            if m:
                pending = (int(m.group(1)), int(m.group(2), 16), int(m.group(3), 16))
                cur = None
                continue
            m = ORG_RE.match(line)
            if m:
                org = int(m.group(1), 16)
                cur = None
                if pending and (org >> 16) == bank and (org & 0xFFFF) == pending[1]:
                    cur = Block(*pending, org)
                    blocks.append(cur)
                pending = None
                continue
            if cur is None:
                continue
            code = line.split(';', 1)[0]
            m = LABEL_RE.match(code)
            if m:
                cur.labels[m.group(1)] = len(cur.mnemonics)
                code = code[m.end():]
                if not code.strip():
                    continue
                code = ' ' + code
            m = INSN_RE.match(code)
            if m:
                cur.mnemonics.append(m.group(1).upper())
    return blocks


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


def sweep(rom: bytes, blk: Block, st: decode.State) -> list[decode.Insn]:
    bank = blk.org & 0xFF0000
    addr, end = blk.org, bank | blk.end
    out = []
    while addr <= end:
        i = decode.decode_insn(rom, addr, st)
        out.append(i)
        st = decode.next_state(i, st)
        addr = i.next_addr
    if addr != end + 1:
        raise decode.DecodeError(f'block ${blk.org:06X}: sweep ends at ${addr:06X}, expected ${end + 1:06X}')
    return out


def main() -> int:
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    cret = os.environ.get('CHRONORET', os.path.join(here, '..', 'ChronoRET'))
    second = os.environ.get('CT_DISASM', os.path.join(here, '..', 'ct_disassembly'))
    rom = decode.load_rom()
    metas = funcs.load()
    errors = []

    by_bank: dict[int, list[Block]] = {}
    for fm in metas:
        bank = fm.addr >> 16
        if bank not in by_bank:
            path = os.path.join(cret, 'asm', f'bank{bank:02X}', f'bank{bank:02X}.asm')
            by_bank[bank] = parse_chronoret(path, bank)

    covered: dict[int, set[int]] = {}
    print(f'{"name":<22} {"addr":>7} {"state":>5} {"size":>4}  {"block":>13} {"blk_size":>8}  result')
    for fm in metas:
        blocks = [b for b in by_bank[fm.addr >> 16] if fm.name in b.labels]
        if len(blocks) != 1:
            errors.append(f'{fm.name}: found in {len(blocks)} ChronoRET blocks')
            continue
        blk = blocks[0]
        k = blk.labels[fm.name]
        for st in fm.entry_states():
            res = []
            sw = sweep(rom, blk, fm.entry_states()[0])
            if [i.mnemonic for i in sw] != blk.mnemonics:
                res.append('block mnemonics differ from source')
            if sum(i.size for i in sw) != blk.size:
                res.append(f'block sweep {sum(i.size for i in sw)} bytes != {blk.size}')
            if sw[k].addr != fm.addr:
                res.append(f'label at ${sw[k].addr:06X} in source, ${fm.addr:06X} in funcs.toml')
            fn = decode.decode_function(rom, fm.addr, st)
            src = blk.mnemonics[k:]
            ret = next(j for j, mn in enumerate(src) if mn in decode.RETURNS)
            if [i.mnemonic for i in fn.insns] != src[:ret + 1]:
                res.append('function mnemonics differ from source')
            lo, hi = fn.extent()
            if hi > (blk.org & 0xFF0000 | blk.end):
                res.append('function runs past block end')
            covered.setdefault(blk.org, set()).update(fn.byte_set())
            tag = f'${blk.start:04X}-${blk.end:04X}'
            print(f'{fm.name:<22} ${fm.addr:06X} {st.tag():>5} {fn.size:>4}  {tag:>13} {blk.size:>8}  '
                  + ('ok' if not res else 'FAIL'))
            errors += [f'{fm.name} {st.tag()}: {r}' for r in res]

    for bank, blocks in by_bank.items():
        for blk in blocks:
            if blk.org in covered and len(covered[blk.org]) != blk.size:
                errors.append(f'block ${blk.org:06X}: functions cover {len(covered[blk.org])} bytes, '
                              f'ChronoRET matched {blk.size}')
    print(f'blocks: {len(covered)}, all decoded-byte unions equal matched size: '
          + ('yes' if not any('block $' in e for e in errors) else 'no'))

    for bank in by_bank:
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

    for e in errors:
        print(f'error: {e}', file=sys.stderr)
    return 1 if errors else 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except decode.DecodeError as ex:
        print(f'check_meta: {ex}', file=sys.stderr)
        sys.exit(1)
