#!/usr/bin/env python3
"""Export what the recompiler knows, per bank, as hints for a matching
disassembly (e.g. ChronoRET).

For every funcs.toml routine that decodes: entry/exit M/X, size, code byte
ranges; every jump table: site, table address, count, targets; every call
boundary. Writes out/hints/bankXX.json and bankXX.txt (the .txt includes
ChronoRET tools/disasm.py commands grouped by entry state).

usage: export_hints.py [--out DIR]
"""
from __future__ import annotations

import json
import os
import sys

GAME = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))   # game/ct
REPO = os.path.dirname(os.path.dirname(GAME))
FUNCS_TOML = os.path.join(GAME, 'funcs.toml')
sys.path.insert(0, os.path.join(REPO, 'recomp'))

import decode  # noqa: E402
import funcs  # noqa: E402


def ranges(addrs: set[int]) -> list[tuple[int, int]]:
    out: list[list[int]] = []
    for a in sorted(addrs):
        if out and a == out[-1][1] + 1:
            out[-1][1] = a
        else:
            out.append([a, a])
    return [(lo, hi) for lo, hi in out]


def main() -> int:
    args = sys.argv[1:]
    out_dir = args[args.index('--out') + 1] if '--out' in args else os.path.join(REPO, 'build', 'hints')
    rom = decode.load_rom()
    metas = funcs.load(FUNCS_TOML)
    reg = funcs.Registry(rom, metas, FUNCS_TOML)
    banks: dict[int, dict] = {}

    def bank(b: int) -> dict:
        return banks.setdefault(b, {'routines': [], 'jumptables': [], 'externs': [], 'code': set(),
                                    'skipped': []})

    for fm in metas:
        for st in fm.entry_states():
            b = bank(fm.addr >> 16)
            try:
                fn = reg.function(fm.addr, st)
            except decode.DecodeError as ex:
                b['skipped'].append({'name': fm.name, 'addr': fm.addr, 'state': st.tag(), 'why': str(ex)})
                continue
            exits = sorted({f'{mn} m{int(m)}x{int(x)}' for mn, m, x in fn.exit_states
                            if m is not None and x is not None})
            b['routines'].append({'name': fm.name, 'addr': fm.addr, 'state': st.tag(),
                                  'exits': exits, 'bytes': fn.size})
            for a in fn.byte_set():
                bank(a >> 16)['code'].add(a)

    for site, count in sorted(reg.jumptables.items()):
        i = decode.decode_insn(rom, site, decode.State(True, False))
        try:
            targets = reg.table_targets(i)
        except decode.DecodeError:
            targets = []
        bank(site >> 16)['jumptables'].append({
            'site': site, 'table': (site & 0xFF0000) | i.operand, 'count': count,
            'targets': sorted(set(targets))})

    for e in reg.externs.values():
        bank(e.addr >> 16)['externs'].append({'name': e.name, 'addr': e.addr, 'kind': e.kind,
                                              'noreturn': e.noreturn})

    os.makedirs(out_dir, exist_ok=True)
    for b, d in sorted(banks.items()):
        code = ranges(d['code'])
        data = {'bank': b, 'routines': sorted(d['routines'], key=lambda r: r['addr']),
                'jumptables': d['jumptables'], 'externs': d['externs'],
                'code_ranges': code, 'skipped': d['skipped']}
        with open(os.path.join(out_dir, f'bank{b:02X}.json'), 'w') as f:
            json.dump(data, f, indent=1)
        lines = [f'; hints for bank ${b:02X} from ct-recomp (funcs.toml + decoder)',
                 f'; {len(data["routines"])} routine entries, {sum(h - l + 1 for l, h in code)} code bytes', '']
        lines.append('; routines: addr state exits bytes name')
        for r in data['routines']:
            lines.append(f'${r["addr"]:06X} {r["state"]} {",".join(r["exits"]) or "noreturn":<24} '
                         f'{r["bytes"]:>5} {r["name"]}')
        lines += ['', '; jump tables: site table count']
        for j in data['jumptables']:
            lines.append(f'${j["site"]:06X} ${j["table"]:06X} {j["count"]}')
        lines += ['', '; code ranges (inclusive)']
        lines += [f'${lo:06X}-${hi:06X}' for lo, hi in code]
        if data['externs']:
            lines += ['', '; call boundaries']
            lines += [f'${e["addr"]:06X} {e["kind"]} {e["name"]}' for e in data['externs']]
        if data['skipped']:
            lines += ['', '; not decoded (see why)']
            lines += [f'${s["addr"]:06X} {s["state"]} {s["name"]}: {s["why"]}' for s in data['skipped']]
        by_state: dict[str, list[str]] = {}
        for r in data['routines']:
            by_state.setdefault(r['state'], []).append(f'{r["addr"] & 0xFFFF:04X}')
        lines += ['', '; ChronoRET: python3 tools/disasm.py <rom> <bank> <entries> --state M?X?E0']
        for st, addrs in sorted(by_state.items()):
            m, x = st[1], st[3]
            for k in range(0, len(addrs), 40):
                lines.append(f'python3 tools/disasm.py roms/chrono_trigger.sfc {b:02X} '
                             f'{" ".join(addrs[k:k + 40])} --state M{m}X{x}E0')
        with open(os.path.join(out_dir, f'bank{b:02X}.txt'), 'w') as f:
            f.write('\n'.join(lines) + '\n')
    total = sum(len(d['routines']) for d in banks.values())
    print(f'export_hints: {len(banks)} banks, {total} routine entries -> {out_dir}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
