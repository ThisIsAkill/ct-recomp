#!/usr/bin/env python3
"""Verify a ROM image against a game's game.toml (size and CRC32).

usage: check_rom.py GAME_DIR [ROM]   (ROM defaults to $CT_ROM)
"""
import os
import sys
import tomllib
import zlib


def check(game_dir: str, path: str) -> str | None:
    with open(os.path.join(game_dir, 'game.toml'), 'rb') as f:
        g = tomllib.load(f)
    if not os.path.isfile(path):
        return f'not a file: {path}'
    size = os.path.getsize(path)
    if size != g['rom_size']:
        return f'size {size}, expected {g["rom_size"]} for {g["name"]}'
    with open(path, 'rb') as f:
        crc = zlib.crc32(f.read())
    if crc != g['rom_crc32']:
        return f'crc32 {crc:08x}, expected {g["rom_crc32"]:08x} for {g["name"]}'
    return None


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__.strip().splitlines()[-1], file=sys.stderr)
        return 2
    path = sys.argv[2] if len(sys.argv) > 2 else os.environ.get('CT_ROM', '')
    if not path:
        print('check_rom: CT_ROM not set', file=sys.stderr)
        return 1
    err = check(sys.argv[1], path)
    if err:
        print(f'check_rom: {err}', file=sys.stderr)
        return 1
    print(f'check_rom: ok ({path})')
    return 0


if __name__ == '__main__':
    sys.exit(main())
