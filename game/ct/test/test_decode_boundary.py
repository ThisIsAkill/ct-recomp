#!/usr/bin/env python3
"""Scheduler boundaries in funcs.toml ([[boundary]]):

The menu thread scheduler ($C28190-$C28248) switches stacks with TCS and
resumes threads through PLD/PLP/PLY/PLX/RTS, which no static stack model
can follow; decoding into it used to die at $C28213 (PLY with unknown X)
or inline the whole scheduler into innocent callers. Checks:

1. funcs.load_boundaries reads the four scheduler addresses, and the
   Registry exposes them.
2. Menu_Fade / Menu_FadeOut ($C283D0 / $C283CA), which JMP to Menu_Yield,
   now stop at the boundary: an interp_tail to $C2821E and no scheduler
   instructions inlined (they used to decode 95/98 instructions).
3. Menu_FadeInThread ($C283C7) stops at Menu_ThreadEnd ($C28216).
4. Menu_Yield / Menu_YieldFrames are returning JSR externs (in the frame
   scheduler the real code runs; the diff hook faults), and stay boundary
   addresses: never function entries.
5. Real thread entries decode: Menu_MainCursorThread ($C2E91B) and
   Menu_MainThread ($C2826C) reach a Menu_Yield extern call.
"""
import os
import sys

GAME = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))   # game/ct
REPO = os.path.dirname(os.path.dirname(GAME))
FUNCS_TOML = os.path.join(GAME, 'funcs.toml')
sys.path.insert(0, os.path.join(REPO, 'recomp'))

import decode  # noqa: E402
import emit  # noqa: E402
import funcs  # noqa: E402

RUN_THREADS_NEXT = 0xC281DA
THREAD_END = 0xC28216
YIELD = 0xC2821E
YIELD_FRAMES = 0xC2822B
FADE = 0xC283D0
FADE_OUT = 0xC283CA
FADE_IN_THREAD = 0xC283C7
MAIN_CURSOR_THREAD = 0xC2E91B
MAIN_THREAD = 0xC2826C


def main() -> int:
    fails = 0

    def check(ok: bool, what: str) -> None:
        nonlocal fails
        if not ok:
            print(f'FAIL: {what}')
            fails += 1

    rom = decode.load_rom()
    bounds = funcs.load_boundaries(FUNCS_TOML)
    check(bounds == {RUN_THREADS_NEXT, THREAD_END, YIELD, YIELD_FRAMES},
          f'boundary table is the scheduler family, got {sorted(hex(b) for b in bounds)}')

    reg = funcs.Registry(rom, funcs.load(FUNCS_TOML), FUNCS_TOML)
    check(reg.boundaries == bounds, 'Registry exposes the boundaries')

    for addr, name in ((FADE, 'Menu_Fade'), (FADE_OUT, 'Menu_FadeOut')):
        fn = reg.function(addr, decode.State(True, False))
        check([t for t, _ in fn.interp_tails.values()] == [YIELD],
              f'{name}: interp_tail to Menu_Yield, got {fn.interp_tails}')
        check(len(fn.insns) < 60,
              f'{name}: scheduler not inlined ({len(fn.insns)} insns, was 95+)')
        check(all(i.addr not in bounds for i in fn.insns[1:]),
              f'{name}: no boundary address decoded as code')

    fn = reg.function(FADE_IN_THREAD, decode.State(False, False))
    check([t for t, _ in fn.interp_tails.values()] == [THREAD_END],
          f'Menu_FadeInThread: interp_tail to Menu_ThreadEnd, got {fn.interp_tails}')

    cst = decode.State(False, False)
    for addr, name in ((YIELD, 'Menu_Yield'), (YIELD_FRAMES, 'Menu_YieldFrames')):
        ext = reg.externs.get(addr)
        check(ext is not None and ext.kind == 'JSR' and not ext.noreturn,
              f'{name}: a returning JSR extern (exit M/X = entry M/X)')
        check(addr in reg.boundaries, f'{name}: never a function entry')

    for addr, name in ((MAIN_CURSOR_THREAD, 'Menu_MainCursorThread'),
                       (MAIN_THREAD, 'Menu_MainThread')):
        fn = reg.function(addr, cst)
        check(YIELD in [t for t, _ in fn.calls.values()],
              f'{name}: reaches a Menu_Yield extern call')

    fm = funcs.FuncMeta('Menu_Fade', FADE, ('m1x0',), 0, 0, 0, 'bankc2', True)
    body = '\n'.join(emit.emit_function(fm, reg.function(FADE, decode.State(True, False))))
    check('ct_tail_rest(cpu, s0);   /* scheduler boundary: its return is ours */' in body,
          'Menu_Fade emission hands off to the interpreter at the boundary')

    print('decode_boundary: ' + ('FAIL' if fails else 'ok'))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
