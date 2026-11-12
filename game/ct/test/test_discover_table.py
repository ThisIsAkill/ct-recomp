#!/usr/bin/env python3
"""discover.py's loop must also seed candidates from jump-table targets a
registered routine dispatches to interpreted (decode.Function.table_interp):
a JSR (abs,X) through a declared [[jumptable]] whose targets are not
registered yet. They are real entry points with a known entry state (the
state at the dispatch site), but they are neither interp_calls nor
MissingTarget sites, so the loop used to never see them."""
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
import discover  # noqa: E402
import funcs  # noqa: E402  (discover put recomp/ on the path)

D, T1, T2 = 0xC01000, 0xC01060, 0xC01080


def w(a: int) -> bytes:
    return bytes([a & 0xFF, (a >> 8) & 0xFF])


def main() -> int:
    rom = bytearray(0x10000)
    code = {
        D: b'\xFC' + w(0x1010) + b'\x60',   # JSR ($1010,X) / RTS
        T1: b'\x60',                        # RTS
        T2: b'\x60',                        # RTS
    }
    for a, c in code.items():
        rom[a & 0xFFFF:(a & 0xFFFF) + len(c)] = c
    rom[0x1010:0x1014] = w(T1) + w(T2)      # the jump table itself
    metas = [funcs.FuncMeta('D', D, ('m1x0',), 0, None, None, 'bankc0')]
    discover.MAX_PASSES = 5
    fails = 0
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, 'funcs.toml')
        with open(path, 'w') as f:
            f.write('[[jumptable]]\nsite = 0xC01000\ncount = 2\n')
        cands, add_states, failures = {}, {}, {}
        passes = []
        discover.log = lambda msg: passes.append(msg)
        _, interp = discover.close_graph(bytes(rom), metas, cands, add_states, failures, path,
                                         {}, {'D'}, 200)
    checks = [
        ('two passes', len(passes), 2),
        ('table targets discovered in m1x0', {a: fm.states for a, fm in cands.items()},
         {T1: ('m1x0',), T2: ('m1x0',)}),
        ('nothing compiled-but-interpreted', interp, {}),
        ('no states added', add_states, {}),
        ('no failures', failures, {}),
    ]
    for what, got, want in checks:
        if got != want:
            print(f'FAIL {what}: got {got}, want {want}')
            fails += 1
    print('test_discover_table: ' + ('ok' if not fails else f'{fails} failed'))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
