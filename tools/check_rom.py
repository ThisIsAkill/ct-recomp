#!/usr/bin/env python3
"""Verify CT_ROM is the headerless US 1.0 image."""
import os
import sys
import zlib

ROM_SIZE = 0x400000
ROM_CRC32 = 0x2D206BF7


def check(path: str) -> str | None:
    if not os.path.isfile(path):
        return f'not a file: {path}'
    size = os.path.getsize(path)
    if size != ROM_SIZE:
        return f'size {size}, expected {ROM_SIZE}'
    with open(path, 'rb') as f:
        crc = zlib.crc32(f.read())
    if crc != ROM_CRC32:
        return f'crc32 {crc:08x}, expected {ROM_CRC32:08x}'
    return None


def main() -> int:
    path = sys.argv[1] if len(sys.argv) > 1 else os.environ.get('CT_ROM', '')
    if not path:
        print('check_rom: CT_ROM not set', file=sys.stderr)
        return 1
    err = check(path)
    if err:
        print(f'check_rom: {err}', file=sys.stderr)
        return 1
    print(f'check_rom: ok ({path})')
    return 0


if __name__ == '__main__':
    sys.exit(main())
