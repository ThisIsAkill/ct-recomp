#!/usr/bin/env python3
"""65816 decoder with static M/X/E tracking.

Opcode table, operand sizing, and HiROM mapping ported from
ChronoRET tools/disasm.py. Reworked as a library: decode one function from
an entry address and entry state into a list of instructions.

Failure policy: any state the decoder cannot resolve statically (unknown
M/X at a width-dependent instruction, conflicting states at one address,
unsupported control flow) raises DecodeError with the address.
"""
from __future__ import annotations

import os
import sys
from dataclasses import dataclass, field

ROM_SIZE = 0x400000


class DecodeError(Exception):
    pass


# ---------------------------------------------------------------------------
# HiROM mapping
# ---------------------------------------------------------------------------
def snes_to_file(addr24: int) -> int | None:
    """File offset for a 24-bit HiROM address, or None if not ROM."""
    bank, addr = addr24 >> 16, addr24 & 0xFFFF
    if bank >= 0xC0:
        return (bank - 0xC0) * 0x10000 + addr
    if 0x40 <= bank <= 0x7D:
        return (bank - 0x40) * 0x10000 + addr
    if bank <= 0x3F or 0x80 <= bank <= 0xBF:
        if addr < 0x8000:
            return None
        return (bank & 0x3F) * 0x10000 + addr
    return None


def load_rom(path: str | None = None) -> bytes:
    path = path or os.environ.get('CT_ROM')
    if not path:
        raise DecodeError('CT_ROM not set')
    with open(path, 'rb') as f:
        rom = f.read()
    if len(rom) != ROM_SIZE:
        raise DecodeError(f'{path}: size {len(rom)}, expected {ROM_SIZE}')
    return rom


# ---------------------------------------------------------------------------
# Opcode table: opcode -> (mnemonic, mode)
# modes: impl, A, rel, rlong, imm, imm_m, imm_x, dp, dp_x, dp_y, dp_ind,
#   dp_x_ind, dp_ind_y, dp_ind_long, dp_ind_long_y, sr, sr_ind_y, abs, abs_x,
#   abs_y, abs_ind, abs_x_ind, abs_ind_long, long, long_x, block
# ---------------------------------------------------------------------------
OPCODES: dict[int, tuple[str, str]] = {
    0x00: ('BRK', 'imm'),
    0x01: ('ORA', 'dp_x_ind'),
    0x02: ('COP', 'imm'),
    0x03: ('ORA', 'sr'),
    0x04: ('TSB', 'dp'),
    0x05: ('ORA', 'dp'),
    0x06: ('ASL', 'dp'),
    0x07: ('ORA', 'dp_ind_long'),
    0x08: ('PHP', 'impl'),
    0x09: ('ORA', 'imm_m'),
    0x0A: ('ASL', 'A'),
    0x0B: ('PHD', 'impl'),
    0x0C: ('TSB', 'abs'),
    0x0D: ('ORA', 'abs'),
    0x0E: ('ASL', 'abs'),
    0x0F: ('ORA', 'long'),
    0x10: ('BPL', 'rel'),
    0x11: ('ORA', 'dp_ind_y'),
    0x12: ('ORA', 'dp_ind'),
    0x13: ('ORA', 'sr_ind_y'),
    0x14: ('TRB', 'dp'),
    0x15: ('ORA', 'dp_x'),
    0x16: ('ASL', 'dp_x'),
    0x17: ('ORA', 'dp_ind_long_y'),
    0x18: ('CLC', 'impl'),
    0x19: ('ORA', 'abs_y'),
    0x1A: ('INC', 'A'),
    0x1B: ('TCS', 'impl'),
    0x1C: ('TRB', 'abs'),
    0x1D: ('ORA', 'abs_x'),
    0x1E: ('ASL', 'abs_x'),
    0x1F: ('ORA', 'long_x'),
    0x20: ('JSR', 'abs'),
    0x21: ('AND', 'dp_x_ind'),
    0x22: ('JSL', 'long'),
    0x23: ('AND', 'sr'),
    0x24: ('BIT', 'dp'),
    0x25: ('AND', 'dp'),
    0x26: ('ROL', 'dp'),
    0x27: ('AND', 'dp_ind_long'),
    0x28: ('PLP', 'impl'),
    0x29: ('AND', 'imm_m'),
    0x2A: ('ROL', 'A'),
    0x2B: ('PLD', 'impl'),
    0x2C: ('BIT', 'abs'),
    0x2D: ('AND', 'abs'),
    0x2E: ('ROL', 'abs'),
    0x2F: ('AND', 'long'),
    0x30: ('BMI', 'rel'),
    0x31: ('AND', 'dp_ind_y'),
    0x32: ('AND', 'dp_ind'),
    0x33: ('AND', 'sr_ind_y'),
    0x34: ('BIT', 'dp_x'),
    0x35: ('AND', 'dp_x'),
    0x36: ('ROL', 'dp_x'),
    0x37: ('AND', 'dp_ind_long_y'),
    0x38: ('SEC', 'impl'),
    0x39: ('AND', 'abs_y'),
    0x3A: ('DEC', 'A'),
    0x3B: ('TSC', 'impl'),
    0x3C: ('BIT', 'abs_x'),
    0x3D: ('AND', 'abs_x'),
    0x3E: ('ROL', 'abs_x'),
    0x3F: ('AND', 'long_x'),
    0x40: ('RTI', 'impl'),
    0x41: ('EOR', 'dp_x_ind'),
    0x42: ('WDM', 'imm'),
    0x43: ('EOR', 'sr'),
    0x44: ('MVP', 'block'),
    0x45: ('EOR', 'dp'),
    0x46: ('LSR', 'dp'),
    0x47: ('EOR', 'dp_ind_long'),
    0x48: ('PHA', 'impl'),
    0x49: ('EOR', 'imm_m'),
    0x4A: ('LSR', 'A'),
    0x4B: ('PHK', 'impl'),
    0x4C: ('JMP', 'abs'),
    0x4D: ('EOR', 'abs'),
    0x4E: ('LSR', 'abs'),
    0x4F: ('EOR', 'long'),
    0x50: ('BVC', 'rel'),
    0x51: ('EOR', 'dp_ind_y'),
    0x52: ('EOR', 'dp_ind'),
    0x53: ('EOR', 'sr_ind_y'),
    0x54: ('MVN', 'block'),
    0x55: ('EOR', 'dp_x'),
    0x56: ('LSR', 'dp_x'),
    0x57: ('EOR', 'dp_ind_long_y'),
    0x58: ('CLI', 'impl'),
    0x59: ('EOR', 'abs_y'),
    0x5A: ('PHY', 'impl'),
    0x5B: ('TCD', 'impl'),
    0x5C: ('JML', 'long'),
    0x5D: ('EOR', 'abs_x'),
    0x5E: ('LSR', 'abs_x'),
    0x5F: ('EOR', 'long_x'),
    0x60: ('RTS', 'impl'),
    0x61: ('ADC', 'dp_x_ind'),
    0x62: ('PER', 'rlong'),
    0x63: ('ADC', 'sr'),
    0x64: ('STZ', 'dp'),
    0x65: ('ADC', 'dp'),
    0x66: ('ROR', 'dp'),
    0x67: ('ADC', 'dp_ind_long'),
    0x68: ('PLA', 'impl'),
    0x69: ('ADC', 'imm_m'),
    0x6A: ('ROR', 'A'),
    0x6B: ('RTL', 'impl'),
    0x6C: ('JMP', 'abs_ind'),
    0x6D: ('ADC', 'abs'),
    0x6E: ('ROR', 'abs'),
    0x6F: ('ADC', 'long'),
    0x70: ('BVS', 'rel'),
    0x71: ('ADC', 'dp_ind_y'),
    0x72: ('ADC', 'dp_ind'),
    0x73: ('ADC', 'sr_ind_y'),
    0x74: ('STZ', 'dp_x'),
    0x75: ('ADC', 'dp_x'),
    0x76: ('ROR', 'dp_x'),
    0x77: ('ADC', 'dp_ind_long_y'),
    0x78: ('SEI', 'impl'),
    0x79: ('ADC', 'abs_y'),
    0x7A: ('PLY', 'impl'),
    0x7B: ('TDC', 'impl'),
    0x7C: ('JMP', 'abs_x_ind'),
    0x7D: ('ADC', 'abs_x'),
    0x7E: ('ROR', 'abs_x'),
    0x7F: ('ADC', 'long_x'),
    0x80: ('BRA', 'rel'),
    0x81: ('STA', 'dp_x_ind'),
    0x82: ('BRL', 'rlong'),
    0x83: ('STA', 'sr'),
    0x84: ('STY', 'dp'),
    0x85: ('STA', 'dp'),
    0x86: ('STX', 'dp'),
    0x87: ('STA', 'dp_ind_long'),
    0x88: ('DEY', 'impl'),
    0x89: ('BIT', 'imm_m'),
    0x8A: ('TXA', 'impl'),
    0x8B: ('PHB', 'impl'),
    0x8C: ('STY', 'abs'),
    0x8D: ('STA', 'abs'),
    0x8E: ('STX', 'abs'),
    0x8F: ('STA', 'long'),
    0x90: ('BCC', 'rel'),
    0x91: ('STA', 'dp_ind_y'),
    0x92: ('STA', 'dp_ind'),
    0x93: ('STA', 'sr_ind_y'),
    0x94: ('STY', 'dp_x'),
    0x95: ('STA', 'dp_x'),
    0x96: ('STX', 'dp_y'),
    0x97: ('STA', 'dp_ind_long_y'),
    0x98: ('TYA', 'impl'),
    0x99: ('STA', 'abs_y'),
    0x9A: ('TXS', 'impl'),
    0x9B: ('TXY', 'impl'),
    0x9C: ('STZ', 'abs'),
    0x9D: ('STA', 'abs_x'),
    0x9E: ('STZ', 'abs_x'),
    0x9F: ('STA', 'long_x'),
    0xA0: ('LDY', 'imm_x'),
    0xA1: ('LDA', 'dp_x_ind'),
    0xA2: ('LDX', 'imm_x'),
    0xA3: ('LDA', 'sr'),
    0xA4: ('LDY', 'dp'),
    0xA5: ('LDA', 'dp'),
    0xA6: ('LDX', 'dp'),
    0xA7: ('LDA', 'dp_ind_long'),
    0xA8: ('TAY', 'impl'),
    0xA9: ('LDA', 'imm_m'),
    0xAA: ('TAX', 'impl'),
    0xAB: ('PLB', 'impl'),
    0xAC: ('LDY', 'abs'),
    0xAD: ('LDA', 'abs'),
    0xAE: ('LDX', 'abs'),
    0xAF: ('LDA', 'long'),
    0xB0: ('BCS', 'rel'),
    0xB1: ('LDA', 'dp_ind_y'),
    0xB2: ('LDA', 'dp_ind'),
    0xB3: ('LDA', 'sr_ind_y'),
    0xB4: ('LDY', 'dp_x'),
    0xB5: ('LDA', 'dp_x'),
    0xB6: ('LDX', 'dp_y'),
    0xB7: ('LDA', 'dp_ind_long_y'),
    0xB8: ('CLV', 'impl'),
    0xB9: ('LDA', 'abs_y'),
    0xBA: ('TSX', 'impl'),
    0xBB: ('TYX', 'impl'),
    0xBC: ('LDY', 'abs_x'),
    0xBD: ('LDA', 'abs_x'),
    0xBE: ('LDX', 'abs_y'),
    0xBF: ('LDA', 'long_x'),
    0xC0: ('CPY', 'imm_x'),
    0xC1: ('CMP', 'dp_x_ind'),
    0xC2: ('REP', 'imm'),
    0xC3: ('CMP', 'sr'),
    0xC4: ('CPY', 'dp'),
    0xC5: ('CMP', 'dp'),
    0xC6: ('DEC', 'dp'),
    0xC7: ('CMP', 'dp_ind_long'),
    0xC8: ('INY', 'impl'),
    0xC9: ('CMP', 'imm_m'),
    0xCA: ('DEX', 'impl'),
    0xCB: ('WAI', 'impl'),
    0xCC: ('CPY', 'abs'),
    0xCD: ('CMP', 'abs'),
    0xCE: ('DEC', 'abs'),
    0xCF: ('CMP', 'long'),
    0xD0: ('BNE', 'rel'),
    0xD1: ('CMP', 'dp_ind_y'),
    0xD2: ('CMP', 'dp_ind'),
    0xD3: ('CMP', 'sr_ind_y'),
    0xD4: ('PEI', 'dp'),
    0xD5: ('CMP', 'dp_x'),
    0xD6: ('DEC', 'dp_x'),
    0xD7: ('CMP', 'dp_ind_long_y'),
    0xD8: ('CLD', 'impl'),
    0xD9: ('CMP', 'abs_y'),
    0xDA: ('PHX', 'impl'),
    0xDB: ('STP', 'impl'),
    0xDC: ('JML', 'abs_ind_long'),
    0xDD: ('CMP', 'abs_x'),
    0xDE: ('DEC', 'abs_x'),
    0xDF: ('CMP', 'long_x'),
    0xE0: ('CPX', 'imm_x'),
    0xE1: ('SBC', 'dp_x_ind'),
    0xE2: ('SEP', 'imm'),
    0xE3: ('SBC', 'sr'),
    0xE4: ('CPX', 'dp'),
    0xE5: ('SBC', 'dp'),
    0xE6: ('INC', 'dp'),
    0xE7: ('SBC', 'dp_ind_long'),
    0xE8: ('INX', 'impl'),
    0xE9: ('SBC', 'imm_m'),
    0xEA: ('NOP', 'impl'),
    0xEB: ('XBA', 'impl'),
    0xEC: ('CPX', 'abs'),
    0xED: ('SBC', 'abs'),
    0xEE: ('INC', 'abs'),
    0xEF: ('SBC', 'long'),
    0xF0: ('BEQ', 'rel'),
    0xF1: ('SBC', 'dp_ind_y'),
    0xF2: ('SBC', 'dp_ind'),
    0xF3: ('SBC', 'sr_ind_y'),
    0xF4: ('PEA', 'abs'),
    0xF5: ('SBC', 'dp_x'),
    0xF6: ('INC', 'dp_x'),
    0xF7: ('SBC', 'dp_ind_long_y'),
    0xF8: ('SED', 'impl'),
    0xF9: ('SBC', 'abs_y'),
    0xFA: ('PLX', 'impl'),
    0xFB: ('XCE', 'impl'),
    0xFC: ('JSR', 'abs_x_ind'),
    0xFD: ('SBC', 'abs_x'),
    0xFE: ('INC', 'abs_x'),
    0xFF: ('SBC', 'long_x'),
}

_OPERAND_BYTES = {
    'impl': 0, 'A': 0, 'imm': 1,
    'dp': 1, 'dp_x': 1, 'dp_y': 1, 'dp_ind': 1, 'dp_x_ind': 1, 'dp_ind_y': 1,
    'dp_ind_long': 1, 'dp_ind_long_y': 1, 'sr': 1, 'sr_ind_y': 1,
    'abs': 2, 'abs_x': 2, 'abs_y': 2, 'abs_ind': 2, 'abs_x_ind': 2,
    'abs_ind_long': 2, 'long': 3, 'long_x': 3, 'rel': 1, 'rlong': 2,
    'block': 2,
}

BRANCHES = {'BPL', 'BMI', 'BVC', 'BVS', 'BCC', 'BCS', 'BNE', 'BEQ'}
RETURNS = {'RTS', 'RTL', 'RTI'}

# Static stack model: push/pull sizes that depend on width use 'm' / 'x'.
_PUSH = {'PHA': 'm', 'PHX': 'x', 'PHY': 'x', 'PHB': 1, 'PHK': 1, 'PHP': 1,
         'PHD': 2, 'PEA': 2, 'PEI': 2, 'PER': 2}
_PULL = {'PLA': 'm', 'PLX': 'x', 'PLY': 'x', 'PLB': 1, 'PLD': 2, 'PLP': 1}


# ---------------------------------------------------------------------------
# CPU state
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class State:
    """Static mode flags. m/x: True = 8-bit, False = 16-bit, None = unknown."""
    m: bool | None
    x: bool | None
    e: bool = False
    # Pushes since entry: (mnemonic, size, (m, x) for PHP else None).
    stack: tuple = ()
    stack_lost: bool = False

    def tag(self) -> str:
        return f"m{_bit(self.m)}x{_bit(self.x)}"

    def key(self) -> tuple:
        return (self.m, self.x, self.e)


def _bit(v: bool | None) -> str:
    return '?' if v is None else ('1' if v else '0')


def parse_state(s: str, e: bool = False) -> State:
    """'m1x0' -> State."""
    s = s.lower()
    if len(s) != 4 or s[0] != 'm' or s[2] != 'x' or s[1] not in '01' or s[3] not in '01':
        raise DecodeError(f'bad state string {s!r}')
    return State(m=s[1] == '1', x=s[3] == '1', e=e)


def default_state(bank: int) -> State:
    """Entry default: native mode, 8-bit A, 16-bit X/Y for bank $C1+
    (ChronoRET session 38). No default elsewhere."""
    if bank >= 0xC1:
        return State(m=True, x=False, e=False)
    raise DecodeError(f'no default entry state for bank ${bank:02X}')


def rep(st: State, v: int) -> State:
    if st.e:
        return st
    return State(False if v & 0x20 else st.m, False if v & 0x10 else st.x,
                 st.e, st.stack, st.stack_lost)


def sep(st: State, v: int) -> State:
    return State(True if v & 0x20 else st.m, True if v & 0x10 else st.x,
                 st.e, st.stack, st.stack_lost)


# ---------------------------------------------------------------------------
# Instruction
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class Insn:
    addr: int            # 24-bit
    opcode: int
    mnemonic: str
    mode: str
    operand: int | None  # raw little-endian operand value
    size: int            # total bytes
    m: bool | None       # state on entry to this instruction
    x: bool | None
    e: bool

    @property
    def next_addr(self) -> int:
        return (self.addr & 0xFF0000) | ((self.addr + self.size) & 0xFFFF)

    def width(self) -> str:
        """Width class used for inventory/emit: '8'/'16' for M- or X-sized
        ops, '' for width-independent ones."""
        dep = width_dependency(self.mnemonic, self.mode)
        if dep is None:
            return ''
        v = self.m if dep == 'm' else self.x
        if v is None:
            raise DecodeError(f'${self.addr:06X}: {self.mnemonic} needs {dep.upper()} width, state unknown')
        return '8' if v else '16'

    def branch_target(self) -> int | None:
        if self.mode == 'rel':
            off = self.operand - 256 if self.operand >= 128 else self.operand
        elif self.mode == 'rlong':
            off = self.operand - 65536 if self.operand >= 32768 else self.operand
        else:
            return None
        return (self.addr & 0xFF0000) | ((self.addr + self.size + off) & 0xFFFF)

    def text(self) -> str:
        return f'{self.mnemonic} {format_operand(self)}'.rstrip()


_M_OPS = {'ADC', 'AND', 'BIT', 'CMP', 'EOR', 'LDA', 'ORA', 'SBC', 'STA', 'STZ',
          'ASL', 'LSR', 'ROL', 'ROR', 'INC', 'DEC', 'TSB', 'TRB', 'PHA', 'PLA'}
_X_OPS = {'CPX', 'CPY', 'LDX', 'LDY', 'STX', 'STY', 'INX', 'INY', 'DEX', 'DEY',
          'PHX', 'PHY', 'PLX', 'PLY', 'MVN', 'MVP'}


def width_dependency(mnemonic: str, mode: str) -> str | None:
    """'m', 'x', or None. Transfers are handled by the emitter per-case."""
    if mnemonic in _M_OPS:
        return 'm'
    if mnemonic in _X_OPS:
        return 'x'
    if mnemonic in ('TAX', 'TAY', 'TXY', 'TYX', 'TSX'):
        return 'x'
    if mnemonic in ('TXA', 'TYA'):
        return 'm'
    return None


def format_operand(i: Insn) -> str:
    op, md = i.operand, i.mode
    if md == 'impl':
        return ''
    if md == 'A':
        return 'A'
    if md == 'imm':
        return f'#${op:02X}'
    if md in ('imm_m', 'imm_x'):
        return f'#${op:0{2 if i.size == 2 else 4}X}'
    fmt = {
        'dp': '${:02X}', 'dp_x': '${:02X},X', 'dp_y': '${:02X},Y',
        'dp_ind': '(${:02X})', 'dp_x_ind': '(${:02X},X)', 'dp_ind_y': '(${:02X}),Y',
        'dp_ind_long': '[${:02X}]', 'dp_ind_long_y': '[${:02X}],Y',
        'sr': '${:02X},S', 'sr_ind_y': '(${:02X},S),Y',
        'abs': '${:04X}', 'abs_x': '${:04X},X', 'abs_y': '${:04X},Y',
        'abs_ind': '(${:04X})', 'abs_x_ind': '(${:04X},X)', 'abs_ind_long': '[${:04X}]',
        'long': '${:06X}', 'long_x': '${:06X},X',
    }
    if md in fmt:
        return fmt[md].format(op)
    if md in ('rel', 'rlong'):
        return f'${i.branch_target() & 0xFFFF:04X}'
    if md == 'block':
        return f'${(op >> 8) & 0xFF:02X},${op & 0xFF:02X}'
    raise DecodeError(f'${i.addr:06X}: unknown mode {md}')


def operand_bytes(mode: str, st: State, addr: int, mnemonic: str) -> int:
    if mode == 'imm_m':
        if st.m is None:
            raise DecodeError(f'${addr:06X}: {mnemonic} immediate needs M width, state unknown')
        return 1 if st.m else 2
    if mode == 'imm_x':
        if st.x is None:
            raise DecodeError(f'${addr:06X}: {mnemonic} immediate needs X width, state unknown')
        return 1 if st.x else 2
    return _OPERAND_BYTES[mode]


def decode_insn(rom: bytes, addr: int, st: State) -> Insn:
    off = snes_to_file(addr)
    if off is None:
        raise DecodeError(f'${addr:06X}: not a ROM address')
    opcode = rom[off]
    mnemonic, mode = OPCODES[opcode]
    n = operand_bytes(mode, st, addr, mnemonic)
    operand = None
    if n:
        # Operand bytes wrap within the bank.
        val = 0
        for k in range(n):
            o = snes_to_file((addr & 0xFF0000) | ((addr + 1 + k) & 0xFFFF))
            val |= rom[o] << (8 * k)
        operand = val
    return Insn(addr, opcode, mnemonic, mode, operand, 1 + n, st.m, st.x, st.e)


# ---------------------------------------------------------------------------
# State transfer
# ---------------------------------------------------------------------------
def _stack_size(kind, st: State, addr: int, mnemonic: str) -> int:
    if kind == 'm':
        if st.m is None:
            raise DecodeError(f'${addr:06X}: {mnemonic} size needs M width, state unknown')
        return 1 if st.m else 2
    if kind == 'x':
        if st.x is None:
            raise DecodeError(f'${addr:06X}: {mnemonic} size needs X width, state unknown')
        return 1 if st.x else 2
    return kind


def next_state(i: Insn, st: State) -> State:
    mn = i.mnemonic
    if mn == 'REP':
        return rep(st, i.operand)
    if mn == 'SEP':
        return sep(st, i.operand)
    if mn == 'XCE':
        raise DecodeError(f'${i.addr:06X}: XCE not supported (carry not tracked)')
    if mn in _PUSH:
        size = _stack_size(_PUSH[mn], st, i.addr, mn)
        saved = (st.m, st.x) if mn == 'PHP' else None
        return State(st.m, st.x, st.e, st.stack + ((mn, size, saved),), st.stack_lost)
    if mn in _PULL:
        size = _stack_size(_PULL[mn], st, i.addr, mn)
        stack, lost = st.stack, st.stack_lost
        top = stack[-1] if stack else None
        if top is not None:
            stack = stack[:-1]
            if top[1] != size:
                stack, lost = (), True
        if mn == 'PLP':
            if top is not None and top[0] == 'PHP' and not lost:
                m, x = top[2]
            else:
                m, x = None, None
            if st.e:
                m, x = True, True
            return State(m, x, st.e, stack, lost)
        return State(st.m, st.x, st.e, stack, lost)
    return st


# ---------------------------------------------------------------------------
# Function decode
# ---------------------------------------------------------------------------
@dataclass
class Function:
    entry: int
    state: State
    insns: list[Insn] = field(default_factory=list)
    exit_states: set = field(default_factory=set)
    calls: dict = field(default_factory=dict)   # call-site addr -> (target, entry State)
    tails: dict = field(default_factory=dict)   # JMP addr -> (target, entry State)

    @property
    def size(self) -> int:
        return sum(i.size for i in self.insns)

    def extent(self) -> tuple[int, int]:
        """(first, last) byte address covered."""
        return self.insns[0].addr, max(i.addr + i.size - 1 for i in self.insns)

    def byte_set(self) -> set[int]:
        out = set()
        for i in self.insns:
            out.update(range(i.addr, i.addr + i.size))
        return out


def decode_function(rom: bytes, entry: int, st: State, resolve=None) -> Function:
    """Decode all instructions reachable from entry without leaving the
    function. In-bank branches are followed. JSR abs requires resolve:
    resolve(site, target, state) -> (m, x) exit state of the callee.
    JMP abs to a registered entry (resolve.tail) is a tail call; any other
    JMP abs is followed as an in-function jump. Other calls and far jumps
    are rejected."""
    fn = Function(entry, st)
    seen: dict[int, tuple] = {}
    work = [(entry, st)]
    while work:
        addr, cur = work.pop()
        while True:
            if addr in seen:
                if seen[addr] != cur.key():
                    raise DecodeError(
                        f'${addr:06X}: reached with {State(*seen[addr]).tag()} and {cur.tag()}')
                break
            i = decode_insn(rom, addr, cur)
            seen[addr] = cur.key()
            fn.insns.append(i)
            nxt = next_state(i, cur)
            mn = i.mnemonic
            if mn in RETURNS:
                fn.exit_states.add((mn, nxt.m, nxt.x))
                break
            if mn in BRANCHES:
                work.append((i.branch_target(), nxt))
            elif mn in ('BRA', 'BRL'):
                addr, cur = i.branch_target(), nxt
                continue
            elif mn == 'JMP' and i.mode == 'abs':
                target = (addr & 0xFF0000) | i.operand
                cst = State(nxt.m, nxt.x, nxt.e)
                exits = resolve.tail(addr, target, cst) if resolve is not None else None
                if exits is not None:
                    fn.tails[addr] = (target, cst)
                    fn.exit_states |= exits
                    break
                addr, cur = target, nxt
                continue
            elif mn == 'JSR' and i.mode == 'abs':
                if resolve is None:
                    raise DecodeError(f'${addr:06X}: {i.text()}: no call resolver')
                target = (addr & 0xFF0000) | i.operand
                m, x = resolve(addr, target, State(nxt.m, nxt.x, nxt.e))
                fn.calls[addr] = (target, State(nxt.m, nxt.x, nxt.e))
                nxt = State(m, x, nxt.e, nxt.stack, nxt.stack_lost)
            elif mn in ('JSR', 'JSL', 'JMP', 'JML', 'BRK', 'COP', 'STP', 'WAI', 'XCE'):
                raise DecodeError(f'${addr:06X}: {i.text()} not supported by decoder yet')
            addr, cur = i.next_addr, nxt
    fn.insns.sort(key=lambda i: i.addr)
    return fn


def listing(fn: Function) -> str:
    rows = []
    for i in fn.insns:
        rows.append(f'${i.addr:06X}  m{_bit(i.m)}x{_bit(i.x)}  {i.text()}')
    return '\n'.join(rows)


def main(argv: list[str]) -> int:
    import argparse
    ap = argparse.ArgumentParser(description='Decode one 65816 function.')
    ap.add_argument('addr', help='24-bit entry address, hex (e.g. C10089)')
    ap.add_argument('--state', help='entry state, e.g. m1x0 (default: bank default)')
    ap.add_argument('--rom', help='ROM path (default: $CT_ROM)')
    a = ap.parse_args(argv)
    rom = load_rom(a.rom)
    addr = int(a.addr, 16)
    st = parse_state(a.state) if a.state else default_state(addr >> 16)
    fn = decode_function(rom, addr, st)
    print(listing(fn))
    print(f'; {len(fn.insns)} insns, {fn.size} bytes')
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main(sys.argv[1:]))
    except DecodeError as ex:
        print(f'decode: {ex}', file=sys.stderr)
        sys.exit(1)
