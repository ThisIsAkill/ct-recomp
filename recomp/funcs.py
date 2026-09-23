"""funcs.toml loader."""
from __future__ import annotations

import os
import tomllib
from dataclasses import dataclass

import decode
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


class Registry:
    """Decodes funcs.toml entries on demand and resolves JSR targets."""

    def __init__(self, rom: bytes, metas: list[FuncMeta]):
        self.rom = rom
        self.by_addr = {fm.addr: fm for fm in metas}
        self.cache: dict[tuple, decode.Function] = {}
        self.active: set[tuple] = set()

    def function(self, addr: int, st: State) -> decode.Function:
        key = (addr, st.m, st.x, st.e)
        if key in self.cache:
            return self.cache[key]
        if key in self.active:
            raise DecodeError(f'${addr:06X}: recursive call chain')
        self.active.add(key)
        try:
            fn = decode.decode_function(self.rom, addr, st, self.resolve)
        finally:
            self.active.discard(key)
        self.cache[key] = fn
        return fn

    def resolve(self, site: int, target: int, st: State) -> tuple:
        fm = self.by_addr.get(target)
        if fm is None:
            raise DecodeError(f'${site:06X}: JSR ${target:06X}: target not in funcs.toml')
        if st.tag() not in fm.states:
            raise DecodeError(f'${site:06X}: JSR {fm.name} with {st.tag()}, '
                              f'funcs.toml lists {", ".join(fm.states)}')
        fn = self.function(target, st)
        exits = {(m, x) for mn, m, x in fn.exit_states if mn == 'RTS'}
        if {mn for mn, _, _ in fn.exit_states} != {'RTS'} or len(exits) != 1:
            raise DecodeError(f'${site:06X}: JSR {fm.name}: exits {sorted(fn.exit_states)}')
        m, x = exits.pop()
        if m is None or x is None:
            raise DecodeError(f'${site:06X}: JSR {fm.name}: exit M/X unknown')
        return m, x
