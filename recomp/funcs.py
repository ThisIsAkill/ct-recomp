"""funcs.toml loader."""
from __future__ import annotations

import os
import tomllib
from dataclasses import dataclass

from decode import DecodeError, State, parse_state

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


@dataclass(frozen=True)
class FuncMeta:
    name: str
    addr: int
    states: tuple[str, ...]
    e: int
    dp: int | None
    db: int | None
    module: str

    def entry_states(self) -> list[State]:
        return [parse_state(s, e=bool(self.e)) for s in self.states]


def load(path: str | None = None) -> list[FuncMeta]:
    path = path or os.path.join(ROOT, 'funcs.toml')
    with open(path, 'rb') as f:
        data = tomllib.load(f)
    out = []
    seen = set()
    for t in data.get('func', []):
        for k in ('name', 'addr', 'states', 'e', 'module'):
            if k not in t:
                raise DecodeError(f'funcs.toml: {t.get("name", "?")}: missing {k}')
        if t['name'] in seen:
            raise DecodeError(f'funcs.toml: duplicate {t["name"]}')
        seen.add(t['name'])
        if t['e']:
            raise DecodeError(f'funcs.toml: {t["name"]}: emulation-mode entry not supported')
        out.append(FuncMeta(t['name'], t['addr'], tuple(t['states']), t['e'],
                            t.get('dp'), t.get('db'), t['module']))
    return out
