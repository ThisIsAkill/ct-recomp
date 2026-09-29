#!/usr/bin/env python3
"""discover.py's discovery loop on a synthetic bank (no game ROM): A and B
call each other (a cycle), A calls N (not registered) and U (registered,
but it ends in a JML to code nobody registered, so its exit M/X is
unknown and its caller runs it interpreted). The loop must settle in two
passes: N becomes a candidate, U is reported as compiled but called
interpreted, and nothing is re-added forever (the loop used to count U's
state, already registered, as new on every pass)."""
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
import discover  # noqa: E402
import funcs  # noqa: E402  (discover put recomp/ on the path)

A, B, U, N = 0xC01000, 0xC01020, 0xC01040, 0xC01060


def w(a: int) -> bytes:
    return bytes([a & 0xFF, (a >> 8) & 0xFF])


def main() -> int:
    rom = bytearray(0x10000)
    code = {
        A: b'\x20' + w(B) + b'\x20' + w(N) + b'\x20' + w(U) + b'\x60',   # JSR B, N, U / RTS
        B: b'\xA5\x10\xF0\x03\x20' + w(A) + b'\x60',   # LDA $10 / BEQ +3 / JSR A / RTS
        U: b'\x5C\x00\x90\xC0',                        # JML $C09000
        N: b'\x60',                                    # RTS
    }
    for a, c in code.items():
        rom[a & 0xFFFF:(a & 0xFFFF) + len(c)] = c
    metas = [funcs.FuncMeta(n, a, ('m1x0',), 0, None, None, 'bankc0')
             for n, a in (('A', A), ('B', B), ('U', U))]
    discover.MAX_PASSES = 5   # the old loop never settled: it would stop here
    fails = 0
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, 'funcs.toml')
        open(path, 'w').close()
        cands, add_states, failures = {}, {}, {}
        passes = []
        discover.log = lambda msg: passes.append(msg)
        _, interp = discover.close_graph(bytes(rom), metas, cands, add_states, failures, path,
                                         {}, {'A', 'B', 'U'}, 200)
    checks = [
        ('two passes', len(passes), 2),
        ('N discovered in m1x0', {a: fm.states for a, fm in cands.items()}, {N: ('m1x0',)}),
        ('U compiled but called interpreted', interp, {('U', 'm1x0'): 1}),
        ('no states added', add_states, {}),
        ('no failures', failures, {}),
    ]
    for what, got, want in checks:
        if got != want:
            print(f'FAIL {what}: got {got}, want {want}')
            fails += 1
    print('test_discover_loop: ' + ('ok' if not fails else f'{fails} failed'))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
