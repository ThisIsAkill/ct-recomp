#!/usr/bin/env python3
"""Close the call graph of funcs.toml.

Decodes every registered routine. Each JSR/JSL to an unregistered target
(or registered without the caller's M/X) -- an interpreter call in the
generated code (#30), or a far jump the decoder rejects -- becomes a
candidate whose entry state is the caller's state at the call site. Repeats until no new targets
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

GAME = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))   # game/ct
REPO = os.path.dirname(os.path.dirname(GAME))
FUNCS_TOML = os.path.join(GAME, 'funcs.toml')
sys.path.insert(0, os.path.join(REPO, 'recomp'))
sys.path.insert(0, os.path.join(GAME, 'tools'))

import check_meta  # noqa: E402
import decode  # noqa: E402
import emit  # noqa: E402
import import_chronoret  # noqa: E402
import funcs  # noqa: E402


def label_names() -> dict[int, str]:
    """addr -> name; ChronoRET labels win over the second disassembly."""
    names: dict[int, str] = {}
    second = os.environ.get('CT_DISASM', os.path.join(REPO, '..', 'ct_disassembly'))
    for path in sorted(glob.glob(os.path.join(second, 'bank_*.asm'))):
        for name, addr in check_meta.parse_second(path).items():
            names.setdefault(addr, name)
    cret = os.environ.get('CHRONORET', os.path.join(REPO, '..', 'ChronoRET'))
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
    metas = funcs.load(FUNCS_TOML)
    names = label_names()
    used_names = {fm.name for fm in metas}
    cands: dict[int, funcs.FuncMeta] = {}
    add_states: dict[str, set] = {}
    failures: dict[tuple, str] = {}
    skipped: list[str] = []
    if '--chronoret' in args:
        bank = int(args[args.index('--chronoret') + 1], 16)
        cret = os.environ.get('CHRONORET', os.path.join(REPO, '..', 'ChronoRET'))
        path = os.path.join(cret, 'asm', f'bank{bank:02X}', f'bank{bank:02X}.asm')
        known = {fm.addr for fm in metas}
        for c in import_chronoret.parse(path):
            if c['addr'] >> 16 != bank or c['addr'] in known:
                continue
            tag, db, _ = import_chronoret.entry_state(c['entry'], bank)
            if tag is None:
                skipped.append(c['name'])
                continue
            cands[c['addr']] = funcs.FuncMeta(c['name'], c['addr'], (tag,), 0, 0, db,
                                              f'bank{bank:02x}')
            used_names.add(c['name'])

    while True:
        reg = funcs.Registry(rom, metas + list(cands.values()), FUNCS_TOML)
        missing: set[tuple] = set()
        failures.clear()
        for fm in metas + list(cands.values()):
            for st in fm.entry_states():
                try:
                    fn = reg.function(fm.addr, st)
                    guessed = decode.assumed_calls(fn)
                    for key, (target, cst) in fn.interp_calls.items():   # runs interpreted (#30)
                        if key in guessed:
                            continue   # its M/X rest on an earlier callee keeping them
                        known = reg.by_addr.get(target)
                        missing.add((target, cst.tag(), known.name if known else None))
                except funcs.MissingTarget as ex:
                    missing.add((ex.target, ex.state.tag(), ex.name))
                except decode.DecodeError as ex:
                    failures[(fm.name, st.tag())] = str(ex)
        new = 0
        for target, tag, known in sorted(missing):
            if known and target not in cands:
                add_states.setdefault(known, set()).add(tag)
                metas = [funcs.FuncMeta(m.name, m.addr, m.states + (tag,), m.e, m.dp, m.db, m.module)
                         if m.name == known and tag not in m.states else m for m in metas]
                new += 1
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

    # Keep only what decodes and emits: a call-site state can come from code
    # after an interpreter call, where M/X are assumed unchanged. Dropping
    # one can break another that relied on it, so repeat until stable.
    base = {m.name: m for m in funcs.load(FUNCS_TOML)}
    ok = list(cands.values())
    while True:
        cur = [funcs.FuncMeta(m.name, m.addr,
                              base[m.name].states + tuple(sorted(add_states.get(m.name, ()))),
                              m.e, m.dp, m.db, m.module, m.manual) if m.name in base else m
               for m in metas]
        reg = funcs.Registry(rom, cur + ok, FUNCS_TOML)
        emit._EXTERNS.clear()
        emit._EXTERNS.update(reg.externs)
        failures.clear()

        def emits(fm, st) -> bool:
            try:
                emit.emit_function(fm, reg.function(fm.addr, st))
                return True
            except (decode.DecodeError, emit.EmitError) as ex:
                failures[(fm.name, st.tag())] = str(ex)
                return False

        new_ok = []
        for fm in ok:
            good = [st.tag() for st in fm.entry_states() if emits(fm, st)]
            if good:
                new_ok.append(funcs.FuncMeta(fm.name, fm.addr, tuple(good), 0, 0, None, fm.module))
        new_add = {}
        for m in cur:
            extra = add_states.get(m.name)
            if not extra:
                continue
            good = {st.tag() for st in m.entry_states() if st.tag() in extra and emits(m, st)}
            if good:
                new_add[m.name] = good
        # registered routines must still emit with the new set
        for m in cur:
            for st in m.entry_states():
                if st.tag() not in add_states.get(m.name, ()) and not m.manual:
                    emits(m, st)
        if len(new_ok) == len(ok) and all(len(f.states) == len(g.states) for f, g in
                                           zip(new_ok, ok)) and new_add == add_states:
            break
        ok, add_states = new_ok, new_add

    for (name, tag), err in sorted(failures.items()):
        print(f'fail  {name} {tag}: {err}')
    for name, tags in sorted(add_states.items()):
        print(f'state {name}: add {", ".join(sorted(tags))}')
    if skipped:
        print(f'# skipped (no documented entry state): {", ".join(skipped)}', file=sys.stderr)
    print(f'# {len(ok)} decodable candidates of {len(cands)}', file=sys.stderr)

    if '--write' in args and add_states:
        path = os.path.join(GAME, 'funcs.toml')
        text = open(path).read()
        for name, tags in add_states.items():
            m = re.search(rf'name = "{re.escape(name)}"\naddr = [^\n]*\nstates = \[([^\]]*)\]', text)
            if not m:
                print(f'error: cannot find states line for {name}', file=sys.stderr)
                return 1
            cur = [s.strip().strip('"') for s in m.group(1).split(',') if s.strip()]
            cur += [s for s in sorted(tags) if s not in cur]
            new_line = ', '.join(f'"{s}"' for s in cur)
            text = text[:m.start(1)] + new_line + text[m.end(1):]
        open(path, 'w').write(text)
    if '--write' in args and ok:
        with open(os.path.join(GAME, 'funcs.toml'), 'a') as f:
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
