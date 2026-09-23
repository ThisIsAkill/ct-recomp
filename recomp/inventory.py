#!/usr/bin/env python3
"""List unique (opcode, mnemonic, mode, width) combinations used by the
functions in funcs.toml."""
from __future__ import annotations

import sys
from collections import Counter

import decode
import funcs


def collect(rom: bytes, metas) -> Counter:
    reg = funcs.Registry(rom, metas)
    c: Counter = Counter()
    for fm in metas:
        for st in fm.entry_states():
            fn = reg.function(fm.addr, st)
            for i in fn.insns:
                c[(i.opcode, i.mnemonic, i.mode, i.width())] += 1
    return c


def main() -> int:
    rom = decode.load_rom()
    c = collect(rom, funcs.load())
    print(f'{"op":>4}  {"mnem":<5} {"mode":<8} {"width":>5}  uses')
    for (op, mn, md, w), n in sorted(c.items()):
        print(f'  {op:02X}  {mn:<5} {md:<8} {w or "-":>5}  {n}')
    print(f'{len(c)} combinations, {len({k[0] for k in c})} opcodes')
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except decode.DecodeError as ex:
        print(f'inventory: {ex}', file=sys.stderr)
        sys.exit(1)
