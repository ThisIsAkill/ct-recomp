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


class MissingTarget(DecodeError):
    """Call target (or its entry state) not registered in funcs.toml."""

    def __init__(self, site: int, target: int, st: State, kind: str, name: str | None = None):
        self.site, self.target, self.state, self.kind, self.name = site, target, st, kind, name
        what = f'{name} has no {st.tag()} entry' if name else 'target not in funcs.toml'
        super().__init__(f'${site:06X}: {kind} ${target:06X} ({st.tag()}): {what}')


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
            fn = decode.decode_function(self.rom, addr, st, self)
        finally:
            self.active.discard(key)
        self.cache[key] = fn
        return fn

    def tail(self, site: int, target: int, st: State) -> set | None:
        """Exit states of a JMP target if it is a registered entry for st."""
        fm = self.by_addr.get(target)
        if fm is None or st.tag() not in fm.states:
            return None
        return set(self.function(target, st).exit_states)

    def __call__(self, site: int, target: int, st: State, kind: str = 'JSR') -> tuple:
        return self.resolve(site, target, st, kind)

    def resolve(self, site: int, target: int, st: State, kind: str = 'JSR') -> tuple:
        """Exit (m, x) of a JSR/JSL callee; it must return only via RTS/RTL."""
        fm = self.by_addr.get(target)
        if fm is None:
            raise MissingTarget(site, target, st, kind)
        if st.tag() not in fm.states:
            raise MissingTarget(site, target, st, kind, fm.name)
        fn = self.function(target, st)
        ret = 'RTS' if kind == 'JSR' else 'RTL'
        exits = {(m, x) for mn, m, x in fn.exit_states if mn == ret}
        if {mn for mn, _, _ in fn.exit_states} != {ret} or len(exits) != 1:
            raise DecodeError(f'${site:06X}: {kind} {fm.name}: exits {sorted(fn.exit_states)}')
        m, x = exits.pop()
        if m is None or x is None:
            raise DecodeError(f'${site:06X}: JSR {fm.name}: exit M/X unknown')
        return m, x
