#!/usr/bin/env python3
"""PLP with unknown provenance (decoder stack model):

A PLP that pops a status byte pushed before entry (e.g. Battle_Bootstrap
PHPs at $CFFB65, tail-calls through BankC1_BattleStartVec into $CFFBE5,
whose epilogue PLPs at $CFFCF4 and RTLs) used to decode with M/X unknown
and fail emission ('PLP restores unknown M/X'), leaving the whole battle
entry chain compiled-but-interpreted. Checks:

1. next_state: a PLP of an in-function PHP byte still restores known M/X.
2. $CFFBE5 (m1x0) decodes past $CFFCF4: the PLP forks into every M/X
   state consistent with the unknown pulled byte (fn.call_exits), and
   every fork reaches the RTL at $CFFCF5.
3. Emission succeeds and dispatches on the runtime M/X after the PLP
   (no static cpu_check_mx against a guessed state).
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

BODY = 0xCFFBE5
PLP = 0xCFFCF4
RTL = 0xCFFCF5


def main() -> int:
    fails = 0

    def check(ok: bool, what: str) -> None:
        nonlocal fails
        if not ok:
            print(f'FAIL: {what}')
            fails += 1

    # 1. In-function PHP -> PLP restores the pushed M/X (no fork).
    st = decode.State(True, False)
    php = decode.Insn(0xC00000, 0x08, 'PHP', 'impl', None, 1, True, False, False)
    st2 = decode.next_state(php, st)
    plp = decode.Insn(0xC00001, 0x28, 'PLP', 'impl', None, 1, False, True, False)
    st3 = decode.next_state(plp, decode.State(False, True, False, st2.stack, st2.stack_lost))
    check((st3.m, st3.x) == (True, False),
          f'PHP/PLP round-trip restores M/X, got m={st3.m} x={st3.x}')

    rom = decode.load_rom()
    reg = funcs.Registry(rom, funcs.load(FUNCS_TOML), FUNCS_TOML)
    fn = reg.function(BODY, decode.State(True, False))

    # 2. The PLP forks; every fork reaches RTL.
    plp_keys = [i.key for i in fn.insns if i.addr == PLP]
    check(len(plp_keys) == 1, f'exactly one PLP insn decoded, got {len(plp_keys)}')
    if plp_keys:
        exits = fn.call_exits.get(plp_keys[0])
        check(exits is not None and len(exits) == 4,
              f'PLP forks into 4 M/X states, got {exits}')
    rtl_states = {(m, x) for (mn, m, x) in fn.exit_states if mn == 'RTL'}
    check(len(rtl_states) == 4,
          f'RTL reached in all 4 forked states, got {sorted(str(s) for s in rtl_states)}')
    check(any(i.addr == RTL for i in fn.insns), 'RTL at $CFFCF5 decoded')

    # 3. Emission: runtime dispatch after PLP, no guessed static check.
    fm = funcs.FuncMeta('Probe_BattleMainBody', BODY, ('m1x0',), 0, 0, 0, 'bankcf', True)
    try:
        body = '\n'.join(emit.emit_function(fm, fn))
    except emit.EmitError as ex:
        body = ''
        check(False, f'emission succeeds, got EmitError: {ex}')
    if body:
        check('cpu->m ==' in body, 'emission dispatches on runtime M/X after PLP')
        check(f'cpu_check_mx(cpu, 0x{PLP:06X}' not in body,
              'no static M/X check against a guessed PLP state')

    print('decode_plp: ' + ('FAIL' if fails else 'ok'))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
