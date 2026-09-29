#!/usr/bin/env python3
"""Regenerate a game's PROGRESS.md from its funcs.toml, the emitter, and ctest,
and the progress summary in README.md (between the progress markers):
functions recompiled out of those known, the share of instructions a
1500-frame boot runs native (BUILD_DIR/ct_boot --profile), and milestones
closed (GitHub, through gh; left out if gh can't answer). Builds BUILD_DIR
first; test results already recorded for this source tree (by an earlier
run, or tools/ctest_cache.py) are reused, not rerun.

usage: progress.py GAME_DIR BUILD_DIR
"""
from __future__ import annotations

import os
import re
import subprocess
import sys
import tomllib

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = ''   # set by main
sys.path.insert(0, os.path.join(ROOT, 'recomp'))

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import ctest_cache  # noqa: E402
import decode  # noqa: E402
import emit  # noqa: E402
import funcs  # noqa: E402


def load_unresolved() -> list[dict]:
    path = os.path.join(GAME, 'unresolved.toml')
    if not os.path.isfile(path):
        return []
    with open(path, 'rb') as f:
        return tomllib.load(f).get('unresolved', [])


def run_tests(build: str) -> tuple[list[tuple[str, str]], int, int, int]:
    """Return ([(test, status)], passed, total, checks). Tests already
    passed on this exact source tree are not rerun (tools/ctest_cache.py)."""
    res, ran = ctest_cache.cached_run(build, os.path.join(build, 'progress_ctest.log'))
    if not res:
        raise SystemExit(f'progress: no tests listed in {build}')
    print(f'progress: {len(res) - len(ran)} test results reused for this tree, '
          f'{len(ran)} run', file=sys.stderr)
    passed = sum(1 for r in res.values() if r['status'] == 'Passed')
    checks = sum(r['checks'] for r in res.values() if r['status'] == 'Passed')
    return [(n, r['status']) for n, r in res.items()], passed, len(res), checks


README_START, README_END = '<!-- progress:start -->', '<!-- progress:end -->'


def boot_native(build: str) -> float | None:
    """Percent of a 1500-frame boot's instructions that ran native."""
    probe = os.path.join(build, 'ct_boot')
    if not os.path.exists(probe):
        return None
    r = subprocess.run([probe, '--frames', '1500', '--profile', '0'], capture_output=True,
                       text=True)
    m = re.search(r'([0-9.]+)% native', r.stdout + r.stderr)
    return float(m.group(1)) if m else None


def milestones() -> tuple[int, int] | None:
    """(closed, total) milestones of the GitHub repo, or None."""
    try:
        r = subprocess.run(['gh', 'api', 'repos/{owner}/{repo}/milestones?state=all',
                            '--jq', '[.[] | .state]'], capture_output=True, text=True,
                           timeout=60, cwd=ROOT)
    except (OSError, subprocess.TimeoutExpired):
        return None
    if r.returncode:
        return None
    states = re.findall(r'"(open|closed)"', r.stdout)
    return (states.count('closed'), len(states)) if states else None


def update_readme(recompiled: int, known: int, native: float | None,
                  ms: tuple[int, int] | None) -> None:
    path = os.path.join(ROOT, 'README.md')
    text = open(path).read()
    if README_START not in text or README_END not in text:
        return
    lines = [README_START, '',
             f'**{100 * recompiled / known:.1f}% recompiled**: {recompiled} of {known} known '
             f'functions are compiled to C.', '']
    if native is not None:
        lines += [f'- Native at runtime: {native:.1f}% of the instructions in a 1500-frame '
                  'boot run as compiled C (the rest run in the interpreter).']
    if ms is not None:
        lines += [f'- Milestones: {ms[0]} of {ms[1]} closed.']
    lines += ['', 'Updated by `tools/progress.py` with every push; details in '
              '[game/ct/PROGRESS.md](game/ct/PROGRESS.md).', '', README_END]
    head, rest = text.split(README_START, 1)
    tail = rest.split(README_END, 1)[1]
    with open(path, 'w') as f:
        f.write(head + '\n'.join(lines) + tail)


def main() -> int:
    global GAME
    if len(sys.argv) != 3:
        print(__doc__.strip().splitlines()[-1], file=sys.stderr)
        return 2
    GAME, build = os.path.abspath(sys.argv[1]), sys.argv[2]
    funcs_toml = os.path.join(GAME, 'funcs.toml')
    rom = decode.load_rom()
    metas = funcs.load(funcs_toml)
    reg = funcs.Registry(rom, metas, funcs_toml)
    metas, pending = funcs.split_emittable(reg, metas)

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

    unresolved = load_unresolved()
    validated_by_bank: dict[int, int] = {}
    for fm in metas:
        b = fm.addr >> 16
        validated_by_bank[b] = validated_by_bank.get(b, 0) + 1
    unresolved_by_bank: dict[int, int] = {}
    for u in unresolved:
        b = u['addr'] >> 16
        unresolved_by_bank[b] = unresolved_by_bank.get(b, 0) + 1
    sync_banks = sorted(set(validated_by_bank) | set(unresolved_by_bank))

    out = ['# Progress', '',
           'Written by `tools/progress.py`. Do not edit by hand.', '',
           '| Metric | Value |', '|---|---|',
           f'| Routines recompiled | {len(metas)} |',
           f'| Emitted C functions (routine x entry state) | {variants} |',
           f'| ROM bytes covered | {len(covered)} |',
           f'| Functions known total (validated + pending + unresolved) | {len(metas) + len(pending) + len(unresolved)} |',
           f'| Functions validated | {len(metas)} |',
           f'| Functions unresolved (pending sync) | {len(unresolved)} |',
           f'| Manual roots not yet emittable | {len(pending)} |',
           f'| Opcodes implemented | {len(ops)} / 256 |',
           f'| Opcodes used by recompiled routines | {len(used)} / 256 |',
           f'| Opcode x width combinations implemented | {combos} |',
           f'| Tests passing | {passed} / {total} |',
           f'| Test assertions checked | {checks} |',
           '', '## Coverage by bank', '', '| Bank | Bytes |', '|---|---|']
    out += [f'| ${b:02X} | {n} |' for b, n in sorted(banks.items())]
    out += ['', '## Symbol sync by bank', '',
            '| Bank | Validated | Unresolved | Known total |', '|---|---|---|---|']
    out += [f'| ${b:02X} | {validated_by_bank.get(b, 0)} | {unresolved_by_bank.get(b, 0)} | '
            f'{validated_by_bank.get(b, 0) + unresolved_by_bank.get(b, 0)} |' for b in sync_banks]
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

    with open(os.path.join(GAME, 'PROGRESS.md'), 'w') as f:
        f.write('\n'.join(out))
    update_readme(len(metas), len(metas) + len(pending) + len(unresolved),
                  boot_native(os.path.abspath(build)), milestones())
    print(f'PROGRESS.md: {len(metas)} routines, {len(covered)} bytes, {len(ops)}/256 opcodes, '
          f'{passed}/{total} tests')
    return 0 if passed == total else 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (decode.DecodeError, emit.EmitError) as ex:
        print(f'progress: {ex}', file=sys.stderr)
        sys.exit(1)
