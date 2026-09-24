#!/usr/bin/env python3
"""Evidence for a (abs,X) jump table's entry count.

Prints: the instructions before the site (to spot CMP/AND bounds), the
table address, the words laid out at the table label in the second
disassembly ('dw' items), and the byte distance to the next code label.

usage: table_extent.py <site hex>
"""
from __future__ import annotations

import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'recomp'))

import decode  # noqa: E402

ADDR_RE = re.compile(r';([0-9A-F]{6})\|')


def main() -> int:
    site = int(sys.argv[1], 16)
    rom = decode.load_rom()
    bank = site >> 16
    path = os.path.join(os.environ.get('CT_DISASM', os.path.join(ROOT, '..', 'ct_disassembly')),
                        f'bank_{bank:02X}.asm')
    lines = open(path).read().splitlines()
    idx = next(k for k, l in enumerate(lines) if f';{site:06X}|' in l)
    print('context:')
    for l in lines[max(0, idx - 12):idx + 1]:
        code = l.split(';')[0].rstrip()
        if code.strip():
            print('   ', code.strip())

    i = decode.decode_insn(rom, site, decode.State(True, False))
    table = (site & 0xFF0000) | i.operand
    print(f'table: ${table:06X}')

    # Words listed as dw from the table's first line.
    start = next((k for k, l in enumerate(lines) if re.search(f';{table:06X}(\||\s*$)', l)), None)
    words = 0
    if start is not None:
        for l in lines[start:]:
            code = l.split(';')[0]
            m = re.search(r'\bdw\b(.*)', code)
            if m:
                words += len([x for x in m.group(1).split(',') if x.strip()])
                continue
            m = re.search(r'\bdb\b(.*)', code)
            if m and '&$FF' in m.group(1):
                words += m.group(1).count('&$FF')   # low byte of each pointer
                continue
            if code.strip() == '' or code.strip().endswith(':') and 'dw' not in code:
                continue
            break
    print(f'pointer items at table label: {words}')

    # Next code address after the table.
    nxt = None
    for l in lines[(start or idx) + 1:]:
        m = ADDR_RE.search(l)
        if m and re.match(r'^\s*[A-Z]{3}\b', l.split(':')[-1].split(';')[0].strip() + ' '):
            a = int(m.group(1), 16)
            if a > table:
                nxt = a
                break
    if nxt:
        print(f'next code: ${nxt:06X} ({(nxt - table) // 2} words of room)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
