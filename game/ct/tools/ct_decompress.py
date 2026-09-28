#!/usr/bin/env python3
"""Chrono Trigger's LZ decompressor (Gfx_DecompressEntry, $C30557), ported
from the ROM's own routine so the build can reproduce what the game writes
into WRAM (overlays, #92).

Stream at SRC (24-bit; all reads stay in SRC's bank, 16-bit wrap):
  word  length of the first segment; the segment ends at SRC + 2 + length
  then groups: a control byte, then one item per bit, low bit first:
    bit 0: one literal byte
    bit 1: a back-reference, two bytes (little-endian word w):
           offset = w & mask, count = (high byte >> shift) + 3,
           copied forward one byte at a time from out[len - offset]
           (so it may overlap what it writes)
  A control byte of 0 instead means 8 literal bytes follow.
  A group covers 8 items, except the first after a segment trailer.
  At the segment end, a trailer: byte t, word e. t & $3F = 0 ends the stream;
  otherwise the next group covers t & $3F items and the next segment ends
  at SRC's low word + e.
The first trailer's top bits pick the variant: 0 -> 12-bit offset, count
from the top 4 bits; otherwise 11-bit offset, count from the top 5 bits.

usage: ct_decompress.py SRC [OUT]   (SRC hex; prints size and FNV-1a 64)
"""
from __future__ import annotations

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..',
                                'recomp'))
import decode  # noqa: E402


class DecompressError(Exception):
    pass


def decompress(rom: bytes, src: int) -> bytes:
    bank, lo = src >> 16, src & 0xFFFF

    def rd(a: int) -> int:
        off = decode.snes_to_file((bank << 16) | (a & 0xFFFF))
        if off is None or off >= len(rom):
            raise DecompressError(f'${src:06X}: read outside ROM at ${bank:02X}{a & 0xFFFF:04X}')
        return rom[off]

    end = (lo + 2 + (rd(lo) | rd(lo + 1) << 8)) & 0xFFFF
    if rd(end) & 0xC0 == 0:
        mask, shift = 0x0FFF, 4
    else:
        mask, shift = 0x07FF, 3
    out = bytearray()
    x = (lo + 2) & 0xFFFF
    count = 8
    while True:
        if x == end:   # segment trailer
            t = rd(x) & 0x3F
            if t == 0:
                return bytes(out)
            count = t
            end = (lo + (rd(x + 1) | rd(x + 2) << 8)) & 0xFFFF
            x = (x + 3) & 0xFFFF
            continue
        c = rd(x)
        if c == 0:   # 8 literals; the group count is left as it was
            out += bytes(rd(x + 1 + k) for k in range(8))
            x = (x + 9) & 0xFFFF
            continue
        x = (x + 1) & 0xFFFF
        while True:
            if c & 1:
                w = rd(x) | rd(x + 1) << 8
                n = (rd(x + 1) >> shift) + 3
                start = len(out) - (w & mask)
                if start < 0:
                    raise DecompressError(f'${src:06X}: back-reference before the output start')
                for k in range(n):
                    out.append(out[start + k])
                x = (x + 2) & 0xFFFF
            else:
                out.append(rd(x))
                x = (x + 1) & 0xFFFF
            c >>= 1
            count -= 1
            if count == 0:
                count = 8
                break
            if len(out) > 0x20000:
                raise DecompressError(f'${src:06X}: output over 128 KB (not a stream?)')


def fnv64(data: bytes) -> int:
    h = 0xCBF29CE484222325
    for b in data:
        h = ((h ^ b) * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h


def main(argv: list[str]) -> int:
    if not argv or len(argv) > 2:
        print(__doc__.strip().splitlines()[-1], file=sys.stderr)
        return 2
    data = decompress(decode.load_rom(), int(argv[0], 16))
    print(f'{len(data)} bytes, fnv64 {fnv64(data):016x}')
    if len(argv) == 2:
        with open(argv[1], 'wb') as f:
            f.write(data)
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main(sys.argv[1:]))
    except DecompressError as ex:
        print(f'ct_decompress: {ex}', file=sys.stderr)
        sys.exit(1)
