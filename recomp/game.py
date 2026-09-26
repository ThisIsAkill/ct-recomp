"""A game directory: game.toml, funcs.toml, unresolved.toml.

The translator and runtime are game-agnostic; everything specific to one
game (symbols, entry-state conventions, ROM identity) lives in its
directory under game/ and is passed in explicitly."""
from __future__ import annotations

import os
import tomllib
from dataclasses import dataclass, field

from decode import DecodeError, State, parse_state


@dataclass
class Game:
    dir: str
    name: str
    rom_size: int
    rom_crc32: int
    mapping: str
    entry_defaults: list[tuple[int, int, str]] = field(default_factory=list)

    @property
    def funcs_toml(self) -> str:
        return os.path.join(self.dir, 'funcs.toml')

    @property
    def unresolved_toml(self) -> str:
        return os.path.join(self.dir, 'unresolved.toml')

    def default_state(self, bank: int) -> State:
        """Entry state assumed for a routine in `bank` with none known."""
        for lo, hi, st in self.entry_defaults:
            if lo <= bank <= hi:
                return parse_state(st)
        raise DecodeError(f'no default entry state for bank ${bank:02X}')


def load(path: str) -> Game:
    """path: the game directory (containing game.toml)."""
    with open(os.path.join(path, 'game.toml'), 'rb') as f:
        t = tomllib.load(f)
    for k in ('name', 'rom_size', 'rom_crc32', 'mapping'):
        if k not in t:
            raise DecodeError(f'{path}/game.toml: missing {k}')
    if t['mapping'] != 'hirom':
        raise DecodeError(f'{path}/game.toml: mapping {t["mapping"]!r} not supported (hirom only)')
    defaults = [(d['banks'][0], d['banks'][1], d['state']) for d in t.get('entry_default', [])]
    return Game(os.path.abspath(path), t['name'], t['rom_size'], t['rom_crc32'], t['mapping'],
                defaults)
