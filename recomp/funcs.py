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


@dataclass(frozen=True)
class Extern:
    """Call boundary: target handled by a runtime hook, not recompiled."""
    name: str
    addr: int
    kind: str      # 'JSR', 'JSL' or 'JML': how it is entered
    hook: str      # C function ct_hook_<hook>(CPU *)
    noreturn: bool = False   # hook never returns (v0 boundary)


def load_externs(path: str | None = None) -> dict[int, Extern]:
    """Exit M/X of an extern equals its entry M/X (checked from the code
    when the entry is declared; see funcs.toml)."""
    path = path or os.path.join(ROOT, 'funcs.toml')
    with open(path, 'rb') as f:
        data = tomllib.load(f)
    out = {}
    for t in data.get('extern', []):
        for k in ('name', 'addr', 'kind', 'hook'):
            if k not in t:
                raise DecodeError(f'funcs.toml: extern {t}: missing {k}')
        if t['kind'] not in ('JSR', 'JSL', 'JML'):
            raise DecodeError(f'funcs.toml: extern {t["name"]}: bad kind')
        if t['kind'] == 'JML' and not t.get('noreturn'):
            raise DecodeError(f'funcs.toml: extern {t["name"]}: JML boundary must be noreturn')
        out[t['addr']] = Extern(t['name'], t['addr'], t['kind'], t['hook'], bool(t.get('noreturn')))
    return out


def load_jumptables(path: str | None = None) -> dict[int, int]:
    """site (24-bit address of JMP/JSR (abs,X)) -> entry count."""
    path = path or os.path.join(ROOT, 'funcs.toml')
    with open(path, 'rb') as f:
        data = tomllib.load(f)
    out = {}
    for t in data.get('jumptable', []):
        if 'site' not in t or 'count' not in t or t['count'] < 1:
            raise DecodeError(f'funcs.toml: bad jumptable {t}')
        out[t['site']] = t['count']
    return out


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


class NeedAssumption(DecodeError):
    """Recursive call reached a routine whose exit state is being computed."""

    def __init__(self, key: tuple):
        self.key = key
        super().__init__(f'${key[0]:06X}: recursive call chain')


class Registry:
    """Decodes funcs.toml entries on demand and resolves JSR targets."""

    def __init__(self, rom: bytes, metas: list[FuncMeta], jumptables: dict[int, int] | None = None):
        self.rom = rom
        self.by_addr = {fm.addr: fm for fm in metas}
        self.jumptables = load_jumptables() if jumptables is None else jumptables
        self.externs = load_externs()
        self.cache: dict[tuple, decode.Function] = {}
        self.active: set[tuple] = set()
        self.assume: dict[tuple, tuple] = {}   # recursive key -> assumed exit (m, x)

    def function(self, addr: int, st: State) -> decode.Function:
        key = (addr, st.m, st.x, st.e)
        if key in self.cache:
            return self.cache[key]
        if key in self.active:
            raise NeedAssumption(key)
        self.active.add(key)
        try:
            fn = decode.decode_function(self.rom, addr, st, self)
        except NeedAssumption as ex:
            if ex.key != key:
                raise
            fn = self._fixpoint(key, addr, st)
        finally:
            self.active.discard(key)
        self.cache[key] = fn
        return fn

    def _fixpoint(self, key: tuple, addr: int, st: State) -> decode.Function:
        """Recursive routine: least fixpoint of its exit states. Start by
        assuming the recursive call never returns, then feed the computed
        exits back until they stop changing."""
        self.assume[key] = set()
        try:
            for _ in range(16):
                before = set(self.cache)
                fn = decode.decode_function(self.rom, addr, st, self)
                if fn.exit_states == self.assume[key]:
                    return fn
                self.assume[key] = set(fn.exit_states)
                for k in set(self.cache) - before:
                    del self.cache[k]
            raise DecodeError(f'${addr:06X}: recursive routine exit states do not converge')
        finally:
            del self.assume[key]

    def table_targets(self, i) -> list[int]:
        """Targets of a (abs,X) jump table, one per even index."""
        count = self.jumptables.get(i.addr)
        if count is None:
            raise DecodeError(f'${i.addr:06X}: {i.text()}: no jumptable entry in funcs.toml')
        bank = i.addr & 0xFF0000
        out = []
        for k in range(count):
            lo = decode.snes_to_file(bank | ((i.operand + 2 * k) & 0xFFFF))
            hi = decode.snes_to_file(bank | ((i.operand + 2 * k + 1) & 0xFFFF))
            if lo is None or hi is None:
                raise DecodeError(f'${i.addr:06X}: jump table not in ROM')
            out.append(bank | self.rom[lo] | self.rom[hi] << 8)
        return out

    def tail_required(self, site: int, target: int, st: State) -> set:
        exits = self.tail(site, target, st)
        if exits is None:
            fm = self.by_addr.get(target)
            raise MissingTarget(site, target, st, 'JMP', fm.name if fm else None)
        return exits

    def tail(self, site: int, target: int, st: State) -> set | None:
        """Exit states of a JMP target if it is a registered entry for st."""
        ext = self.externs.get(target)
        if ext is not None:
            if ext.kind != 'JML' or not ext.noreturn:
                raise DecodeError(f'${site:06X}: jump to extern {ext.name} declared {ext.kind}')
            return set()
        fm = self.by_addr.get(target)
        if fm is None or st.tag() not in fm.states:
            return None
        key = (target, st.m, st.x, st.e)
        if key in self.assume:
            return set(self.assume[key])
        return set(self.function(target, st).exit_states)

    def __call__(self, site: int, target: int, st: State, kind: str = 'JSR') -> tuple:
        return self.resolve(site, target, st, kind)

    def resolve(self, site: int, target: int, st: State, kind: str = 'JSR') -> set:
        """Exit {(m, x)} of a JSR/JSL callee; it must return only via RTS/RTL."""
        ext = self.externs.get(target)
        if ext is not None:
            if ext.kind != kind:
                raise DecodeError(f'${site:06X}: {kind} to extern {ext.name} declared {ext.kind}')
            return set() if ext.noreturn else {(st.m, st.x)}
        fm = self.by_addr.get(target)
        if fm is None:
            raise MissingTarget(site, target, st, kind)
        if st.tag() not in fm.states:
            raise MissingTarget(site, target, st, kind, fm.name)
        key = (target, st.m, st.x, st.e)
        ret = 'RTS' if kind == 'JSR' else 'RTL'
        exit_states = self.assume[key] if key in self.assume else self.function(target, st).exit_states
        exits = {(m, x) for mn, m, x in exit_states if mn == ret}
        if not exit_states:
            return set()   # never returns
        if {mn for mn, _, _ in exit_states} != {ret}:
            raise DecodeError(f'${site:06X}: {kind} {fm.name}: exits {sorted(exit_states, key=str)}')
        if any(m is None or x is None for m, x in exits):
            raise DecodeError(f'${site:06X}: {kind} {fm.name}: exit M/X unknown')
        return exits
