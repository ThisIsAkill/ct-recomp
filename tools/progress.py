#!/usr/bin/env python3
"""Regenerate PROGRESS.md from funcs.toml, the emitter, and ctest."""
from __future__ import annotations

import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'recomp'))

import decode  # noqa: E402
import emit  # noqa: E402
import funcs  # noqa: E402


def run_tests(build: str) -> tuple[list[tuple[str, str]], int, int, int]:
    """Return ([(test, status)], passed, total, checks)."""
    p = subprocess.run(['ctest', '--test-dir', build, '-V'], capture_output=True, text=True)
    out = p.stdout
    results = re.findall(r'^\s*\d+/\d+ Test\s+#\d+: (\S+) \.+\s*(\*{0,3}\w+)', out, re.M)
    checks = sum(int(n) for n in re.findall(r'^\d+: \S+: (\d+) checks, 0 failed$', out, re.M))
    m = re.search(r'(\d+)% tests passed, (\d+) tests failed out of (\d+)', out)
    if not m:
        raise SystemExit(f'progress: no ctest summary from {build}\n{out[-2000:]}{p.stderr}')
    total = int(m.group(3))
    return results, total - int(m.group(2)), total, checks


def main() -> int:
    build = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, 'build')
    rom = decode.load_rom()
    metas = funcs.load()
    reg = funcs.Registry(rom, metas)

    rows, covered, variants, used = [], set(), 0, set()
    modules = emit.modules(metas)
    for mod in modules:
        emit.emit_module(reg, metas, mod)   # raises if anything is unimplemented
    for fm in metas:
        sizes = []
        for st in fm.entry_states():
            fn = reg.function(fm.addr, st)
            covered |= fn.byte_set()
            used |= {i.opcode for i in fn.insns}
            sizes.append(fn.size)
            variants += 1
        rows.append((fm, sizes))

    ops = emit.implemented_opcodes()
    combos = len(emit.TEMPLATES)
    tests, passed, total, checks = run_tests(build)

    banks: dict[int, int] = {}
    for a in covered:
        banks[a >> 16] = banks.get(a >> 16, 0) + 1

    out = ['# Progress', '',
           'Written by `tools/progress.py`. Do not edit by hand.', '',
           '| Metric | Value |', '|---|---|',
           f'| Routines recompiled | {len(metas)} |',
           f'| Emitted C functions (routine x entry state) | {variants} |',
           f'| ROM bytes covered | {len(covered)} |',
           f'| Opcodes implemented | {len(ops)} / 256 |',
           f'| Opcodes used by recompiled routines | {len(used)} / 256 |',
           f'| Opcode x width combinations implemented | {combos} |',
           f'| Tests passing | {passed} / {total} |',
           f'| Test assertions checked | {checks} |',
           '', '## Coverage by bank', '', '| Bank | Bytes |', '|---|---|']
    out += [f'| ${b:02X} | {n} |' for b, n in sorted(banks.items())]
    out += ['', '## Routines', '', '| Routine | Address | Entry states | Bytes | Module |',
            '|---|---|---|---|---|']
    for fm, sizes in rows:
        size = str(sizes[0]) if len(set(sizes)) == 1 else '/'.join(map(str, sizes))
        out.append(f'| {fm.name} | ${fm.addr:06X} | {", ".join(fm.states)} | {size} | {fm.module} |')
    out += ['', '## Implemented opcodes', '',
            ', '.join(f'${o:02X} {decode.OPCODES[o][0]} {decode.OPCODES[o][1]}' for o in sorted(ops)),
            '', '## Tests', '', '| Test | Result |', '|---|---|']
    out += [f'| {name} | {status.strip("*")} |' for name, status in tests]
    out.append('')

    with open(os.path.join(ROOT, 'PROGRESS.md'), 'w') as f:
        f.write('\n'.join(out))
    print(f'PROGRESS.md: {len(metas)} routines, {len(covered)} bytes, {len(ops)}/256 opcodes, '
          f'{passed}/{total} tests')
    return 0 if passed == total else 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (decode.DecodeError, emit.EmitError) as ex:
        print(f'progress: {ex}', file=sys.stderr)
        sys.exit(1)
