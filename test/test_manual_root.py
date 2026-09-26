#!/usr/bin/env python3
"""Manual roots in funcs.toml (`manual = true`):

1. funcs.load() reads the flag (Field_ColdBootInit, $C0000E).
2. A manual root seeds sync_symbols.py's M/X propagation: a candidate it
   calls with no documented state gets the state live at the call site.
3. split_emittable() holds a manual root back from emission while it
   doesn't decode, with the reason; the same entry without the flag still
   fails emission loudly.
"""
import dataclasses
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'recomp'))
sys.path.insert(0, os.path.join(ROOT, 'tools'))

import decode  # noqa: E402
import emit  # noqa: E402
import funcs  # noqa: E402
import sync_symbols  # noqa: E402

COLD_BOOT = 0xC0000E
LOAD_LOCATION = 0xC000F4   # JSR'd from the location loop the root falls into


def main() -> int:
    fails = 0

    def check(ok: bool, what: str) -> None:
        nonlocal fails
        if not ok:
            print(f'FAIL: {what}')
            fails += 1

    rom = decode.load_rom()
    metas = funcs.load()
    root = next((fm for fm in metas if fm.addr == COLD_BOOT), None)
    check(root is not None and root.manual, 'Field_ColdBootInit is a manual root')
    check(root is not None and (root.states, root.dp, root.db) == (('m1x0',), 0x2100, 0x00),
          'Field_ColdBootInit entry state m1x0, DP=$2100, DB=$00')
    check(all(not fm.manual for fm in metas if fm.addr != COLD_BOOT), 'no other manual roots')
    if root is None:
        return 1

    cand = sync_symbols.Candidate(LOAD_LOCATION, 'Field_LoadLocationResources', 'test')
    seeded = sync_symbols.propagate(rom, [root], {LOAD_LOCATION: cand})
    check(seeded[LOAD_LOCATION].states == ('m1x0',),
          f'root seeds propagation: got {seeded[LOAD_LOCATION].states}')

    reg = funcs.Registry(rom, metas)
    ok, pending = funcs.split_emittable(reg, metas)
    check(root not in ok, 'undecodable manual root is not emitted')
    check([fm for fm, _ in pending] == [root], 'only the manual root is pending')
    check(bool(pending) and 'JSL $C70000' in pending[0][1], f'pending reason: {pending}')

    plain = dataclasses.replace(root, manual=False)
    preg = funcs.Registry(rom, [plain])
    pok, ppending = funcs.split_emittable(preg, [plain])
    check(pok == [plain] and not ppending, 'non-manual entry is never held back')
    try:
        emit.emit_module(preg, pok, plain.module)
        check(False, 'non-manual undecodable entry fails emission')
    except emit.EmitError:
        pass

    print(f'manual_root: {"ok" if not fails else f"{fails} failed"}')
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
