#!/usr/bin/env python3
"""List ChronoRET routines for a bank and try to decode each one.

For every 'org' label in asm/bank<BB>/bank<BB>.asm: entry state from the
nearest preceding 'Entry:' comment (M=, X=, DB=; missing X uses the bank
default), then decode through the funcs.toml registry extended with all
candidates. Prints a status per routine and, with --toml, funcs.toml
entries for those that decode.

usage: import_chronoret.py <bank hex> [--module NAME] [--toml]
"""
from __future__ import annotations

import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'recomp'))

import decode  # noqa: E402
import funcs  # noqa: E402

ORG_RE = re.compile(r'^org \$([0-9A-F]{6})', re.I)
LABEL_RE = re.compile(r'^([A-Za-z_][A-Za-z0-9_]*):')
ENTRY_RE = re.compile(r'^;\s*Entry:(.*)', re.I)


def parse(path: str) -> list[dict]:
    out = []
    entry_note = None
    pending_org = None
    with open(path) as f:
        lines = f.readlines()
    for n, line in enumerate(lines):
        m = ENTRY_RE.match(line)
        if m:
            entry_note = m.group(1)
            continue
        m = ORG_RE.match(line)
        if m:
            pending_org = int(m.group(1), 16)
            continue
        m = LABEL_RE.match(line)
        if m and pending_org is not None:
            out.append({'name': m.group(1), 'addr': pending_org, 'entry': entry_note, 'line': n + 1})
            pending_org = None
            entry_note = None
            continue
        if line.strip() and not line.startswith(';') and pending_org is None:
            entry_note = entry_note if line.startswith((' ', '\t', '.')) else entry_note
    return out


def entry_state(note: str | None, bank: int) -> tuple[str | None, int | None, str]:
    """(state tag, DB, source); tag None when neither documented nor defaulted."""
    try:
        base = decode.default_state(bank)
        m, x, src = base.m, base.x, 'default'
    except decode.DecodeError:
        m, x, src = None, None, 'none'
    db = None
    if note:
        mm = re.search(r'\bM\s*=\s*([01])', note)
        mx = re.search(r'\bX\s*=\s*([01])', note)
        md = re.search(r'\bDB\s*=\s*\$([0-9A-F]{2})', note, re.I)
        if mm:
            m, src = mm.group(1) == '1', 'doc'
        if mx:
            x = mx.group(1) == '1'
        if md:
            db = int(md.group(1), 16)
    if m is None or x is None:
        return None, db, src
    return f"m{int(m)}x{int(x)}", db, src


def main() -> int:
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        return 2
    bank = int(args[0], 16)
    module = args[args.index('--module') + 1] if '--module' in args else f'c{bank:02x}_auto'.lower()
    want_toml = '--toml' in args
    cret = os.environ.get('CHRONORET', os.path.join(ROOT, '..', 'ChronoRET'))
    path = os.path.join(cret, 'asm', f'bank{bank:02X}', f'bank{bank:02X}.asm')
    cands = [c for c in parse(path) if c['addr'] >> 16 == bank]

    rom = decode.load_rom()
    metas = funcs.load()
    known = {fm.addr for fm in metas}
    extra = []
    for c in cands:
        st, db, src = entry_state(c['entry'], bank)
        c.update(state=st or '-', db=db, src=src)
        if st is not None and c['addr'] not in known:
            extra.append(funcs.FuncMeta(c['name'], c['addr'], (st,), 0, 0, db, module))

    # Fixpoint: drop candidates that fail, since callers may depend on them.
    live = list(extra)
    status: dict[int, str] = {}
    while True:
        reg = funcs.Registry(rom, metas + live)
        failed = []
        for fm in live:
            try:
                reg.function(fm.addr, fm.entry_states()[0])
                status[fm.addr] = 'ok'
            except decode.DecodeError as ex:
                status[fm.addr] = str(ex)
                failed.append(fm)
        if not failed:
            break
        live = [fm for fm in live if fm not in failed]

    for c in cands:
        s = 'in funcs.toml' if c['addr'] in known else status.get(c['addr'], '?')
        print(f"{c['addr']:06X} {c['state']} {c['src']:<7} {c['name']:<40} {s}")
    ok = [fm for fm in live]
    print(f'# {len(ok)} decodable of {len(extra)} new candidates', file=sys.stderr)
    if want_toml:
        for fm in ok:
            print(f'\n[[func]]\nname = "{fm.name}"\naddr = 0x{fm.addr:06X}\n'
                  f'states = ["{fm.states[0]}"]\ne = 0\ndp = 0x0000\n'
                  + (f'db = 0x{fm.db:02X}\n' if fm.db is not None else '')
                  + f'module = "{fm.module}"')
    return 0


if __name__ == '__main__':
    sys.exit(main())
