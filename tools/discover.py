#!/usr/bin/env python3
"""Close the call graph of funcs.toml.

Decodes every registered routine. Each JSR/JSL to an unregistered target
(or registered without the caller's M/X) becomes a candidate whose entry
state is the caller's state at the call site. Repeats until no new targets
appear or --max candidates exist. Names: ChronoRET label, else second
disassembly label, else Sub_XXXXXX.

usage: discover.py [--chronoret BANK] [--max N] [--write]
  --chronoret  also seed with the bank's ChronoRET routines not yet registered
  --write  append decodable candidates to funcs.toml (module bankXX);
           state additions for existing entries are reported, not written
"""
from __future__ import annotations

import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'recomp'))
sys.path.insert(0, os.path.join(ROOT, 'tools'))

import check_meta  # noqa: E402
import decode  # noqa: E402
import import_chronoret  # noqa: E402
import funcs  # noqa: E402


def label_names() -> dict[int, str]:
    """addr -> name; ChronoRET labels win over the second disassembly."""
    names: dict[int, str] = {}
    second = os.environ.get('CT_DISASM', os.path.join(ROOT, '..', 'ct_disassembly'))
    for path in sorted(glob.glob(os.path.join(second, 'bank_*.asm'))):
        for name, addr in check_meta.parse_second(path).items():
            names.setdefault(addr, name)
    cret = os.environ.get('CHRONORET', os.path.join(ROOT, '..', 'ChronoRET'))
    org_re = re.compile(r'^org \$([0-9A-F]{6})', re.I)
    lab_re = re.compile(r'^([A-Za-z_][A-Za-z0-9_]*):')
    for path in sorted(glob.glob(os.path.join(cret, 'asm', 'bank*', '*.asm'))):
        org = None
        with open(path) as f:
            for line in f:
                m = org_re.match(line)
                if m:
                    org = int(m.group(1), 16)
                    continue
                m = lab_re.match(line)
                if m and org is not None:
                    names[org] = m.group(1)
                    org = None
                elif line.strip() and not line.startswith(';'):
                    org = None
    return names


def main() -> int:
    args = sys.argv[1:]
    limit = int(args[args.index('--max') + 1]) if '--max' in args else 200
    rom = decode.load_rom()
    metas = funcs.load()
    names = label_names()
    used_names = {fm.name for fm in metas}
    cands: dict[int, funcs.FuncMeta] = {}
    add_states: dict[str, set] = {}
    failures: dict[tuple, str] = {}
    if '--chronoret' in args:
        bank = int(args[args.index('--chronoret') + 1], 16)
        cret = os.environ.get('CHRONORET', os.path.join(ROOT, '..', 'ChronoRET'))
        path = os.path.join(cret, 'asm', f'bank{bank:02X}', f'bank{bank:02X}.asm')
        known = {fm.addr for fm in metas}
        for c in import_chronoret.parse(path):
            if c['addr'] >> 16 != bank or c['addr'] in known:
                continue
            tag, db, _ = import_chronoret.entry_state(c['entry'], bank)
            cands[c['addr']] = funcs.FuncMeta(c['name'], c['addr'], (tag,), 0, 0, db,
                                              f'bank{bank:02x}')
            used_names.add(c['name'])

    while True:
        reg = funcs.Registry(rom, metas + list(cands.values()))
        missing: set[tuple] = set()
        failures.clear()
        for fm in metas + list(cands.values()):
            for st in fm.entry_states():
                try:
                    reg.function(fm.addr, st)
                except funcs.MissingTarget as ex:
                    missing.add((ex.target, ex.state.tag(), ex.name))
                except decode.DecodeError as ex:
                    failures[(fm.name, st.tag())] = str(ex)
        new = 0
        for target, tag, known in sorted(missing):
            if known and target not in cands:
                add_states.setdefault(known, set()).add(tag)
                continue
            if target in cands:
                fm = cands[target]
                if tag not in fm.states:
                    cands[target] = funcs.FuncMeta(fm.name, fm.addr, fm.states + (tag,), 0, 0, None,
                                                   fm.module)
                    new += 1
                continue
            if len(cands) >= limit:
                continue
            name = names.get(target, f'Sub_{target:06X}')
            if name in used_names:
                name = f'{name}_{target:06X}'
            used_names.add(name)
            cands[target] = funcs.FuncMeta(name, target, (tag,), 0, 0, None, f'bank{target >> 16:02x}')
            new += 1
        if not new:
            break

    reg = funcs.Registry(rom, metas + list(cands.values()))
    ok = []
    for fm in cands.values():
        good = []
        for st in fm.entry_states():
            try:
                reg.function(fm.addr, st)
                good.append(st.tag())
            except decode.DecodeError as ex:
                failures[(fm.name, st.tag())] = str(ex)
        if good:
            ok.append(funcs.FuncMeta(fm.name, fm.addr, tuple(good), 0, 0, None, fm.module))

    for (name, tag), err in sorted(failures.items()):
        print(f'fail  {name} {tag}: {err}')
    for name, tags in sorted(add_states.items()):
        print(f'state {name}: add {", ".join(sorted(tags))} to funcs.toml')
    print(f'# {len(ok)} decodable candidates of {len(cands)}', file=sys.stderr)

    if '--write' in args and ok:
        with open(os.path.join(ROOT, 'funcs.toml'), 'a') as f:
            f.write('\n# ---- discovered by tools/discover.py (entry state from call sites) ----\n')
            for fm in sorted(ok, key=lambda m: m.addr):
                states = ', '.join(f'"{s}"' for s in fm.states)
                f.write(f'\n[[func]]\nname = "{fm.name}"\naddr = 0x{fm.addr:06X}\n'
                        f'states = [{states}]\ne = 0\nmodule = "{fm.module}"\n')
    else:
        for fm in sorted(ok, key=lambda m: m.addr):
            print(f'ok    {fm.name:<40} ${fm.addr:06X} {",".join(fm.states)}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
