#!/usr/bin/env python3
"""Sync function symbols for one bank from dscotton/ct_disassembly and
ChronoRET into funcs.toml, validated against recomp/decode.py.

Sources
-------
dscotton/ct_disassembly (env DSCOTTON_DIR): one flat listing per bank,
`bank_XX.asm`. Every code line carries its own address/bytes/branch-target
in a trailing comment column: `<insn>  ;AAAAAA|BYTES  |TARGET;text`. There
is no separate `org` directive per label and no documented entry M/X state
per label -- SEP/REP are ordinary decoded instructions, and `.B`/`.W`/`.L`
mnemonic suffixes just pick the immediate/operand width the assembler
needed, which happens to mirror the real M/X state at that point but isn't
a declared fact we can extract without decoding. A labelled line is either
`Name: <insn> ...;AAAAAA|...` (address on the same line) or a bare `Name:`
whose address comes from the next line that carries one (a pure alias).
Two label conventions matter here:
  - `_<BANK>L<addr>` (e.g. `_C0L005D`) -- an in-function branch target,
    not a routine. Excluded.
  - everything else, including auto-numbered `CODE_<addr>` -- a genuine
    label (named or not). A label is only kept as a candidate if the
    token right after it is a real 65816 mnemonic (from decode.OPCODES);
    this drops data/pointer tables (`dw ...`) that share the same
    labelled-line shape.

ChronoRET (../ChronoRET relative to this repo, or env CHRONORET):
`asm/bank<BB>/bank<BB>.asm`, one `org $AAAAAA` per label followed by
`Name:`. Entry M/X, when known, is documented in free-text comments in
the block immediately above (`Entry: M=1, X=0`, `On entry: M=1, X=0 ...`,
or a parenthetical mid-sentence `Entry M=1, X=1`) -- there's no fixed
phrasing, so we scan the contiguous comment run above `org` for the last
`M=`/`X=` pair rather than anchoring on one exact prefix.

Merge
-----
By address. A ChronoRET name/state always wins over dscotton's. Where
ChronoRET has no comment-documented state, dscotton contributes nothing
useful there either (it never documents state) -- those candidates start
with an unknown entry state and are only resolved by propagation (below).

Validation
----------
recomp/decode.py (via recomp/funcs.py's Registry) requires every callee of
a function to already be a registered entry with a matching state tag
before the *caller* can be decoded at all -- so we cannot "decode and see"
our way to an undocumented callee's entry state; decoding never reaches
past the first unregistered call. Instead:

1. Propagation: a lightweight, resolver-free walk (reuses decode_insn /
   next_state, not decode_function) over every routine whose entry state
   is already known -- existing funcs.toml entries plus documented/
   already-propagated candidates -- recording the M/X in effect at every
   JSR/JSL/JSR-table site it passes through. That's a fact about the
   *caller*, independent of whether the callee itself decodes, so it
   needs no resolver. Iterated to a fixpoint (a newly-stated candidate's
   own body is walked next, in case it calls further undocumented ones).
   A candidate seen under more than one state keeps all of them (some
   routines legitimately have several valid entry variants); this is a
   hint, not a fact -- wrong guesses are simply rejected in step 2.
2. Real validation: recomp/funcs.py's Registry, exactly as
   tools/import_chronoret.py already does it for one source. Build a
   registry from the existing funcs.toml plus every candidate that now
   has a state (documented or propagated); try to decode each; drop any
   that raise DecodeError and retry, to a fixpoint (since one candidate's
   failure can only be diagnosed once its siblings have either dropped
   out or settled). What survives is validated. What never got a state,
   or never decoded, goes to unresolved.toml with a reason -- never into
   funcs.toml.

Output
------
funcs.toml keeps every existing entry byte-for-byte. New, validated
entries are written into a single delimited, address-sorted block per
bank (`# ==== sync_symbols: bank $XX (auto) ====` ... `# ==== end ... `),
replacing any previous block for that same bank so re-running the sync
after a source update updates in place. Re-running with unchanged inputs
reproduces the same block byte-for-byte (tested by test/test_sync.py).
An address already present under a *different* name in the existing file
(hand-written or from a previous sync) is left untouched and reported as
a conflict on stderr, never silently overwritten.

usage: sync_symbols.py <bank hex> [--module NAME] [--dry-run]
"""
from __future__ import annotations

import argparse
import os
import re
import sys
from dataclasses import dataclass, field

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'recomp'))

import decode  # noqa: E402
import funcs  # noqa: E402
from decode import DecodeError, State  # noqa: E402

FUNCS_TOML = os.path.join(ROOT, 'funcs.toml')
UNRESOLVED_TOML = os.path.join(ROOT, 'unresolved.toml')

MNEMONICS = {m for m, _ in decode.OPCODES.values()}

# decode.py's static validation (terminates, entry state known) is not a
# full correctness proof: it can't see that a routine's actual runtime
# control flow doesn't match the RTS-discipline it assumed. Caught here by
# the existing ctest `diff_all` differential suite, not by this tool's own
# checks -- addresses confirmed bad that way are excluded permanently so
# re-running sync doesn't re-add them; a fix belongs in decode.py/emit.py
# or in re-classifying the routine, not in silently dropping this list.
KNOWN_BAD: dict[int, str] = {
    0xC0AF4E: ("diff_all: generated code times out (step budget exhausted at "
               "$C0B0E1) while the interpreter detects a corrupted return "
               "address inside $C0AF50-$C0B0FF on the same random input -- "
               "real control-flow divergence, not a missing feature"),
    0xC0B096: ("diff_all: same $C0B0E1 divergence as Field_CopyMapRectLayers "
               "($C0AF4E); the two likely share the broken code path"),
    0xC0A521: ("diff_all: Map_BuildTilePropGrid -- generated code loops to a "
               "step-budget timeout at $C0A5C3 (a BCC closing a copy loop) on "
               "input the interpreter runs to completion on; looks like an "
               "emit.py bug specific to this loop, not decode.py or a missing "
               "opcode -- needs the generated C at $C0A5C3 compared by hand "
               "against interp.c's handling of the same instruction"),
}


def dscotton_dir() -> str:
    d = os.environ.get('DSCOTTON_DIR')
    if not d or not os.path.isdir(d):
        raise SystemExit(f'DSCOTTON_DIR not set or missing (got {d!r}); '
                          'point it at a dscotton/ct_disassembly checkout')
    return d


def chronoret_dir() -> str:
    d = os.environ.get('CHRONORET', os.path.join(ROOT, '..', 'ChronoRET'))
    if not os.path.isdir(d):
        raise SystemExit(f'ChronoRET checkout not found at {d!r}; '
                          'set CHRONORET or place it at ../ChronoRET')
    return d


@dataclass(frozen=True)
class Candidate:
    addr: int
    name: str
    src: str                 # 'chronoret' | 'dscotton'
    states: tuple[str, ...] = ()   # documented/propagated so far


# ---------------------------------------------------------------------------
# dscotton
# ---------------------------------------------------------------------------
_LOCAL_LABEL_RE = re.compile(r'^_[0-9A-F]{2}L[0-9A-F]+$')
_LABEL_LINE_RE = re.compile(r'^\s*([A-Za-z_][A-Za-z0-9_]*):(.*)$')
_ADDR_COL_RE = re.compile(r';([0-9A-F]{6})\|')


def _mnemonic(text: str) -> str | None:
    """First token of the instruction part of a line (before the address
    comment column), base mnemonic with any .B/.W/.L suffix stripped."""
    head = text.split(';', 1)[0].strip()
    if not head:
        return None
    tok = head.split()[0].split('.')[0].upper()
    return tok or None


def parse_dscotton(bank: int) -> dict[int, str]:
    """addr -> name, for every dscotton label that is real code in `bank`
    and not an in-function branch target."""
    path = os.path.join(dscotton_dir(), f'bank_{bank:02X}.asm')
    if not os.path.isfile(path):
        raise SystemExit(f'no dscotton listing for bank ${bank:02X}: {path}')
    with open(path, encoding='utf-8', errors='replace') as f:
        lines = f.readlines()

    out: dict[int, str] = {}
    n = len(lines)
    i = 0
    while i < n:
        m = _LABEL_LINE_RE.match(lines[i])
        if not m:
            i += 1
            continue
        name, rest = m.group(1), m.group(2)
        if _LOCAL_LABEL_RE.match(name):
            i += 1
            continue
        addr_m = _ADDR_COL_RE.search(rest)
        if addr_m:
            mnem = _mnemonic(rest)
            j = i
        else:
            # Bare alias label: address/mnemonic come from the next line
            # that actually carries an address column.
            j = i + 1
            addr_m = None
            mnem = None
            while j < n:
                addr_m = _ADDR_COL_RE.search(lines[j])
                if addr_m:
                    mnem = _mnemonic(lines[j])
                    break
                if _LABEL_LINE_RE.match(lines[j]) is None and lines[j].strip():
                    break
                j += 1
        if addr_m and mnem in MNEMONICS:
            addr = int(addr_m.group(1), 16)
            if addr >> 16 == bank:
                out.setdefault(addr, name)
        i += 1
    return out


# ---------------------------------------------------------------------------
# ChronoRET
# ---------------------------------------------------------------------------
_ORG_RE = re.compile(r'^org\s+\$([0-9A-F]{6})', re.I)
_LABEL_ONLY_RE = re.compile(r'^([A-Za-z_][A-Za-z0-9_]*):')
_STATE_RE = re.compile(r'\bM\s*=\s*([01]).{0,40}?\bX\s*=\s*([01])|\bX\s*=\s*([01]).{0,40}?\bM\s*=\s*([01])',
                        re.I | re.S)
_DIVIDER_RE = re.compile(r'^;\s*=+\s*$')


def _relevant_comment_text(lines: list[str], idx: int) -> str:
    """Text describing the `org` at `lines[idx]`: the contiguous run of
    comment/blank lines directly above it, bounded to whichever divider
    ('; ===...===') section is nearest -- either the text after the last
    divider in that run (an un-closed trailing note, as in the label-stub
    section), or, if that's empty because the last divider is a closing
    one right above `org`, the section between the two innermost dividers
    (a self-contained per-function header block). Without this bound, a
    contiguous run reaches all the way up through a bank-wide header
    ("CPU state on entry from MainInit...") and would misattribute the
    bank's boot-time state to whatever the first `org` in the file is."""
    j = idx - 1
    block = []
    while j >= 0 and (lines[j].strip() == '' or lines[j].lstrip().startswith(';')):
        block.append(lines[j])
        j -= 1
    block.reverse()
    dividers = [i for i, l in enumerate(block) if _DIVIDER_RE.match(l)]
    if not dividers:
        return ''.join(block)
    trailing = block[dividers[-1] + 1:]
    if any(l.strip() not in ('', ';') for l in trailing):
        return ''.join(trailing)
    if len(dividers) >= 2:
        return ''.join(block[dividers[-2] + 1:dividers[-1]])
    return ''.join(trailing)


def parse_chronoret(bank: int) -> dict[int, tuple[str, str | None]]:
    """addr -> (name, state tag or None), for every `org`/label pair in
    ChronoRET's listing for `bank` (if one exists there yet)."""
    path = os.path.join(chronoret_dir(), 'asm', f'bank{bank:02X}', f'bank{bank:02X}.asm')
    if not os.path.isfile(path):
        return {}
    with open(path, encoding='utf-8', errors='replace') as f:
        lines = f.readlines()

    out: dict[int, tuple[str, str | None]] = {}
    pending_addr = None
    pending_org_idx = None
    for idx, line in enumerate(lines):
        m = _ORG_RE.match(line)
        if m:
            pending_addr = int(m.group(1), 16)
            pending_org_idx = idx
            continue
        m = _LABEL_ONLY_RE.match(line)
        if m and pending_addr is not None:
            addr = pending_addr
            pending_addr = None
            if addr >> 16 != bank:
                continue
            block = _relevant_comment_text(lines, pending_org_idx)
            sm = None
            for sm in _STATE_RE.finditer(block):
                pass  # last match in the block wins
            state = None
            if sm:
                if sm.group(1) is not None:
                    mbit, xbit = sm.group(1), sm.group(2)
                else:
                    xbit, mbit = sm.group(3), sm.group(4)
                state = f'm{mbit}x{xbit}'
            out[addr] = (m.group(1), state)
    return out


# ---------------------------------------------------------------------------
# Merge
# ---------------------------------------------------------------------------
def merge(bank: int) -> dict[int, Candidate]:
    dsc = parse_dscotton(bank)
    cret = parse_chronoret(bank)
    out: dict[int, Candidate] = {}
    for addr, name in dsc.items():
        out[addr] = Candidate(addr, name, 'dscotton')
    for addr, (name, state) in cret.items():
        states = (state,) if state else ()
        out[addr] = Candidate(addr, name, 'chronoret', states)
    return out


# ---------------------------------------------------------------------------
# Propagation: what state is X called with, anywhere we can already see?
# ---------------------------------------------------------------------------
def _call_site_states(rom: bytes, entry: int, st: State) -> dict[int, set[str]]:
    """Resolver-free walk of one routine from a known entry state, purely
    to see what state is live at each JSR/JSL/table-JSR site. Assumes a
    call returns with the state it was made in (true for essentially all
    hand-written 65816 code); a wrong assumption only ever produces a
    candidate state that step-2 validation will reject."""
    found: dict[int, set[str]] = {}
    seen: set[tuple] = set()
    work = [(entry, st)]
    guard = 0
    while work and guard < 20000:
        addr, cur = work.pop()
        while True:
            guard += 1
            key = (addr, cur.m, cur.x, cur.e)
            if key in seen or guard >= 20000:
                break
            seen.add(key)
            try:
                i = decode.decode_insn(rom, addr, cur)
            except DecodeError:
                break
            nxt = decode.next_state(i, cur)
            mn = i.mnemonic
            if mn in decode.RETURNS:
                break
            if mn in decode.BRANCHES:
                tgt = i.branch_target()
                if tgt is not None:
                    work.append((tgt, nxt))
            elif mn in ('BRA', 'BRL'):
                tgt = i.branch_target()
                if tgt is None:
                    break
                addr, cur = tgt, nxt
                continue
            elif mn in ('JSR', 'JSL') and i.mode in ('abs', 'long'):
                target = (addr & 0xFF0000 | i.operand) if mn == 'JSR' else i.operand
                if nxt.m is not None and nxt.x is not None:
                    found.setdefault(target, set()).add(f'm{int(nxt.m)}x{int(nxt.x)}')
            elif mn in ('JML',) and i.mode == 'long':
                break
            elif mn == 'JMP' and i.mode == 'abs':
                break   # tail call target already resolved elsewhere
            elif mn in ('JSR', 'JMP') and i.mode == 'abs_x_ind':
                break   # jump-table dispatch, not a plain call site
            elif mn in ('BRK', 'COP', 'STP', 'WAI'):
                break
            addr, cur = i.next_addr, nxt
    return found


def propagate(rom: bytes, existing: list[funcs.FuncMeta],
               candidates: dict[int, Candidate]) -> dict[int, Candidate]:
    """Fixpoint: seed from every already-known routine (existing entries
    plus documented candidates), and keep re-walking any candidate that
    newly gained a state, until nothing changes."""
    cands = dict(candidates)
    known: list[tuple[int, State]] = [(fm.addr, s) for fm in existing for s in fm.entry_states()]
    for c in cands.values():
        for s in c.states:
            known.append((c.addr, decode.parse_state(s)))

    walked: set[tuple[int, str]] = set()
    changed = True
    while changed:
        changed = False
        todo, known = known, []
        for addr, st in todo:
            wk = (addr, st.tag())
            if wk in walked:
                continue
            walked.add(wk)
            for target, tags in _call_site_states(rom, addr, st).items():
                c = cands.get(target)
                if c is None:
                    continue
                new_tags = tags - set(c.states)
                if not new_tags:
                    continue
                c = Candidate(c.addr, c.name, c.src, tuple(sorted(set(c.states) | new_tags)))
                cands[target] = c
                changed = True
                for tag in new_tags:
                    known.append((target, decode.parse_state(tag)))
    return cands


# ---------------------------------------------------------------------------
# Validation
# ---------------------------------------------------------------------------
@dataclass
class Result:
    validated: list[funcs.FuncMeta] = field(default_factory=list)
    unresolved: list[tuple[Candidate, str]] = field(default_factory=list)
    conflicts: list[tuple[Candidate, str]] = field(default_factory=list)


def validate(rom: bytes, existing: list[funcs.FuncMeta], candidates: dict[int, Candidate],
             module: str) -> Result:
    res = Result()
    existing_by_addr = {fm.addr: fm for fm in existing}

    live: dict[int, Candidate] = {}
    for addr, c in candidates.items():
        if addr in KNOWN_BAD:
            res.unresolved.append((c, KNOWN_BAD[addr]))
            continue
        ex = existing_by_addr.get(addr)
        if ex is not None:
            if ex.name != c.name:
                res.conflicts.append((c, f'address already registered as {ex.name!r}'))
            continue
        if not c.states:
            res.unresolved.append((c, 'entry M/X state unknown: not documented, '
                                       'no caller found within synced scope'))
            continue
        live[addr] = c

    reasons: dict[int, str] = {}
    while True:
        metas = list(existing) + [
            funcs.FuncMeta(c.name, c.addr, c.states, 0, None, None, module) for c in live.values()
        ]
        reg = funcs.Registry(rom, metas)
        failed = []
        for addr, c in live.items():
            try:
                for s in c.states:
                    reg.function(addr, decode.parse_state(s))
            except DecodeError as ex:
                reasons[addr] = str(ex)
                failed.append(addr)
        if not failed:
            break
        for addr in failed:
            del live[addr]

    for addr, c in sorted(live.items()):
        res.validated.append(funcs.FuncMeta(c.name, c.addr, c.states, 0, None, None, module))
    for addr, c in candidates.items():
        if addr in reasons and addr not in existing_by_addr:
            res.unresolved.append((c, reasons[addr]))
    res.unresolved.sort(key=lambda t: t[0].addr)
    res.conflicts.sort(key=lambda t: t[0].addr)
    return res


# ---------------------------------------------------------------------------
# funcs.toml / unresolved.toml I/O
# ---------------------------------------------------------------------------
def _block_markers(bank: int) -> tuple[str, str]:
    return (f'# ==== sync_symbols: bank ${bank:02X} (auto) ====\n',
            f'# ==== end sync_symbols: bank ${bank:02X} ====\n')


def _func_toml(fm: funcs.FuncMeta) -> str:
    states = ', '.join(f'"{s}"' for s in fm.states)
    return (f'[[func]]\nname = "{fm.name}"\naddr = 0x{fm.addr:06X}\n'
            f'states = [{states}]\ne = 0\nmodule = "{fm.module}"\n')


def write_funcs_toml(bank: int, validated: list[funcs.FuncMeta], dry_run: bool,
                      path: str = FUNCS_TOML) -> None:
    start, end = _block_markers(bank)
    with open(path, encoding='utf-8') as f:
        text = f.read()
    if start in text:
        pre, rest = text.split(start, 1)
        _, post = rest.split(end, 1)
        text = pre + post
    text = text.rstrip('\n') + '\n'
    if validated:
        block = start + '\n' + '\n'.join(_func_toml(fm) for fm in sorted(validated, key=lambda f: f.addr)) + '\n' + end
        text = text + '\n' + block
    if dry_run:
        print(f'--- funcs.toml would gain {len(validated)} entries for bank ${bank:02X} ---')
    else:
        with open(path, 'w', encoding='utf-8') as f:
            f.write(text)


def _unresolved_toml(c: Candidate, reason: str) -> str:
    return (f'[[unresolved]]\nname = "{c.name}"\naddr = 0x{c.addr:06X}\n'
            f'src = "{c.src}"\nreason = "{reason}"\n')


def write_unresolved_toml(bank: int, unresolved: list[tuple[Candidate, str]], dry_run: bool,
                           path: str = UNRESOLVED_TOML) -> None:
    start, end = _block_markers(bank)
    text = ''
    if os.path.isfile(path):
        with open(path, encoding='utf-8') as f:
            text = f.read()
    if start in text:
        pre, rest = text.split(start, 1)
        _, post = rest.split(end, 1)
        text = pre + post
    text = text.rstrip('\n')
    text = (text + '\n\n') if text else ''
    if unresolved:
        block = start + '\n' + '\n'.join(_unresolved_toml(c, r) for c, r in unresolved) + '\n' + end
        text = text + block + '\n'
    if dry_run:
        print(f'--- unresolved.toml would have {len(unresolved)} entries for bank ${bank:02X} ---')
    else:
        with open(path, 'w', encoding='utf-8') as f:
            f.write(text)


def sync(bank: int, module: str | None = None) -> Result:
    """Run the full sync for `bank` and return the result, without writing
    anything -- used by callers (and tests) that want to inspect or diff
    output themselves."""
    module = module or f'bank{bank:02x}'
    rom = decode.load_rom()
    existing = funcs.load()
    candidates = merge(bank)
    candidates = propagate(rom, existing, candidates)
    return validate(rom, existing, candidates, module)


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser()
    p.add_argument('bank', help='bank in hex, e.g. C0')
    p.add_argument('--module', default=None)
    p.add_argument('--dry-run', action='store_true')
    p.add_argument('--funcs-toml', default=FUNCS_TOML)
    p.add_argument('--unresolved-toml', default=UNRESOLVED_TOML)
    a = p.parse_args(argv)
    bank = int(a.bank, 16)
    module = a.module or f'bank{bank:02x}'
    result = sync(bank, module)

    for c, reason in result.conflicts:
        print(f'CONFLICT ${c.addr:06X} {c.name} ({c.src}): {reason}', file=sys.stderr)
    for c, reason in result.unresolved:
        print(f'unresolved ${c.addr:06X} {c.name} ({c.src}): {reason}', file=sys.stderr)
    print(f'bank ${bank:02X}: {len(result.validated)} validated, '
          f'{len(result.unresolved)} unresolved, {len(result.conflicts)} conflicts',
          file=sys.stderr)

    write_funcs_toml(bank, result.validated, a.dry_run, a.funcs_toml)
    write_unresolved_toml(bank, result.unresolved, a.dry_run, a.unresolved_toml)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
