#!/usr/bin/env python3
"""recomp/decode.py on synthetic bytes (no ROM): calls to code that isn't
compiled become interpreter calls (#30), with the continuation decoded in
the call's own M/X; decode.assumed_calls marks the calls whose M/X rest on
such an assumption until a REP/SEP grounds them; a JMP to the routine's own
entry is a loop, not a tail call; WDM executes as a NOP; code in WRAM decodes
from its image (decode.Image) and nowhere else; jump tables of unknown
length and JMLs to uncompiled code go through the interpreter's dispatch."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'recomp'))
import decode  # noqa: E402
import emit  # noqa: E402
import funcs  # noqa: E402

fails = 0


def check(ok, what):
    global fails
    if not ok:
        fails += 1
        print('FAIL', what)


class Resolver:
    """Every call target is missing (runs interpreted); $E40000 is a
    registered entry for JMP tail calls."""
    externs = {}

    def missing(self, target, st, kind):
        return True

    def tail(self, site, target, st):
        return {('RTS', st.m, st.x)} if target == 0xE40000 else None


BANK = 0xE4   # synthetic code at $E48000 (HiROM file offset $248000)


def rom_with(code: bytes, at: int = 0x8000) -> bytes:
    rom = bytearray((BANK - 0xC0 + 1) * 0x10000)
    base = (BANK - 0xC0) * 0x10000 + at
    rom[base:base + len(code)] = code
    return bytes(rom)


m1x0 = decode.State(True, False)

# $E48000: JSR $9000 / JSR $9100 / REP #$30 / JSR $9200 / RTS
rom = rom_with(bytes([0x20, 0x00, 0x90, 0x20, 0x00, 0x91, 0xC2, 0x30, 0x20, 0x00, 0x92, 0x60]))
fn = decode.decode_function(rom, 0xE48000, m1x0, Resolver())
sites = {k[0]: t for k, (t, _) in fn.interp_calls.items()}
check(sites == {0xE48000: 0xE49000, 0xE48003: 0xE49100, 0xE48008: 0xE49200},
      f'three interpreter calls: {sites}')
check(not fn.calls, 'no compiled calls')
cont = [i for i in fn.insns if i.addr == 0xE48003]
check(len(cont) == 1 and (cont[0].m, cont[0].x) == (True, False),
      'continuation decoded in the caller\'s state')
assumed = {k[0] for k in decode.assumed_calls(fn)}
check(assumed == {0xE48003}, f'only the call after an interpreter call is assumed: {assumed}')

# $E48000: loop: DEX / BNE +2 / RTS / JMP $8000  (the jump back to the entry)
rom = rom_with(bytes([0xCA, 0xD0, 0x01, 0x60, 0x4C, 0x00, 0x80]))
fn = decode.decode_function(rom, 0xE48000, m1x0, Resolver())
check(not fn.tails, f'jump to the own entry is a loop, not a tail call: {fn.tails}')
rom = rom_with(bytes([0x4C, 0x00, 0x00]))
fn = decode.decode_function(rom, 0xE48000, m1x0, Resolver())
check(len(fn.tails) == 1, 'jump to another registered entry is a tail call')

# WDM ($42 + signature byte) is reserved on the 65C816 and executes as a
# two-byte NOP in both engines, so it decodes and emits like one.
rom = rom_with(bytes([0xEA, 0x42, 0x00, 0x60]))
fn = decode.decode_function(rom, 0xE48000, m1x0, Resolver())
check([i.mnemonic for i in fn.insns] == ['NOP', 'WDM', 'RTS'],
      f'WDM decodes as NOP: {[i.mnemonic for i in fn.insns]}')
check(0x42 in emit.implemented_opcodes(), 'WDM: emitter implements opcode')

# RTI ($40) ends a handler; the emitter returns through op_rti.
fn = decode.decode_function(rom_with(bytes([0x40])), 0xE48000, m1x0, Resolver())
check(('RTI', True, False) in fn.exit_states, f'RTI exit: {fn.exit_states}')
check(0x40 in emit.implemented_opcodes(), 'RTI: emitter implements opcode')
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('rti_handler', 0xE48000, ('m1x0',), 0, None, None, 'test'), fn))
check('op_rti(cpu, 0xE48000);' in body, 'RTI: emitter returns through op_rti')

# MVP ($44, block move down) with 16-bit indexes emits mvp16 like MVN's mvn16.
fn = decode.decode_function(rom_with(bytes([0x44, 0x7E, 0x7F, 0x60])), 0xE48000,
                            decode.State(True, False), Resolver())
check([i.mnemonic for i in fn.insns] == ['MVP', 'RTS'], 'MVP decodes')
check(0x44 in emit.implemented_opcodes(), 'MVP: emitter implements opcode')
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('mvp_move', 0xE48000, ('m1x0',), 0, None, None, 'test'), fn))
check('mvp16(cpu, 0xE48000, 0x7E, 0x7F);' in body, f'MVP: emitter emits mvp16: {body}')

# MVN/MVP with 8-bit indexes (X flag set): the index registers step masked
# to 8 bits (as the interpreter's block_move does), so the emitter needs
# the mvn8/mvp8 forms, not a "width 8 not implemented" failure.
m1x1 = decode.State(True, True)
fn = decode.decode_function(rom_with(bytes([0x54, 0x7E, 0x7F, 0x60])), 0xE48000,
                            m1x1, Resolver())
check([i.mnemonic for i in fn.insns] == ['MVN', 'RTS'], 'MVN (x1) decodes')
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('mvn_move8', 0xE48000, ('m1x1',), 0, None, None, 'test'), fn))
check('mvn8(cpu, 0xE48000, 0x7E, 0x7F);' in body, f'MVN (x1): emitter emits mvn8: {body}')
fn = decode.decode_function(rom_with(bytes([0x44, 0x7E, 0x7F, 0x60])), 0xE48000,
                            m1x1, Resolver())
check([i.mnemonic for i in fn.insns] == ['MVP', 'RTS'], 'MVP (x1) decodes')
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('mvp_move8', 0xE48000, ('m1x1',), 0, None, None, 'test'), fn))
check('mvp8(cpu, 0xE48000, 0x7E, 0x7F);' in body, f'MVP (x1): emitter emits mvp8: {body}')

# JML [$9000] ($DC): target is a 24-bit bank-0 pointer known only at run
# time, so decode marks it dynamic and emit reads the pointer and dispatches.
fn = decode.decode_function(rom_with(bytes([0xDC, 0x00, 0x90])), 0xE48000, m1x0, Resolver())
check(list(fn.dyn_tables) and ('?', None, None) in fn.exit_states,
      'JML (abs_ind_long): dynamic, exits unknown')
check(0xDC in emit.implemented_opcodes(), 'JML (abs_ind_long): emitter implements opcode')
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('jml_ind', 0xE48000, ('m1x0',), 0, None, None, 'test'), fn))
check('read16_b0(0x9000)' in body and 'cpu->PB = (uint8_t)(t >> 16);' in body and
      'ct_tail_rest(cpu, s0);' in body,
      'JML (abs_ind_long): emitter reads bank-0 pointer and dispatches')

# Code in WRAM (an overlay image, #92): $7E4000: JSR $4010 / BRA +1 / RTS,
# where $7E4010 (in the image) runs interpreted; nothing past the image is read.
img = decode.Image(rom_with(b''), 0x7E4000, bytes([0x20, 0x10, 0x40, 0x80, 0x00, 0x60]))
fn = decode.decode_function(img, 0x7E4000, m1x0, Resolver())
check([i.addr for i in fn.insns] == [0x7E4000, 0x7E4003, 0x7E4005], 'decoded from the image')
check({k[0]: t for k, (t, _) in fn.interp_calls.items()} == {0x7E4000: 0x7E4010},
      'call inside the image runs interpreted')
try:
    decode.decode_function(decode.Image(rom_with(b''), 0x7E4000, bytes([0xEA])), 0x7E4000,
                           m1x0, Resolver())
    check(False, 'running off the image end is rejected')
except decode.DecodeError:
    pass

# A patched call site (#97): a JSR whose operand the game rewrites decodes
# with no static target, and the emitter reads the target from the operand
# bytes at run time and dispatches it (the bytes stay out of the hash).
img = decode.Image(rom_with(b''), 0x7E4000, bytes([0x20, 0x10, 0x40, 0x60]))
fn = decode.decode_function(img, 0x7E4000, m1x0, Resolver(), patched={0x7E4000})
check({k[0] for k in fn.patched_calls} == {0x7E4000} and not fn.interp_calls,
      f'patched JSR: no static target: {fn.patched_calls}')
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('patched_jsr', 0x7E4000, ('m1x0',), 0, None, None, 'test'), fn))
check('bus_peek(0x7E0000u | 0x4001)' in body and 'ct_call_interp(cpu, t, 0x7E4003, 2)' in body,
      f'patched JSR: emitter reads the operand and dispatches: {body}')
check(emit.fnv64_skip(bytes([0xA9, 0x42, 0x85, 0x15, 0x60]), {1}) == 0x3C1A2ED7CA8F7496,
      'fnv64_skip agrees with runtime/overlay.c matches (test_native hash)')

# Tables of unknown length and jumps to code that isn't compiled.
class Dyn(Resolver):
    jumptables = {}

# $E48000: JSR ($9000,X) / RTS: dispatched at run time, continuation decoded
fn = decode.decode_function(rom_with(bytes([0xFC, 0x00, 0x90, 0x60])), 0xE48000, m1x0, Dyn())
check(list(fn.dyn_tables) and [i.addr for i in fn.insns] == [0xE48000, 0xE48003],
      'JSR (abs,X) of unknown length: dynamic, continuation decoded')
# JMP ($9000,X): the path ends, what it returns with is unknown
fn = decode.decode_function(rom_with(bytes([0x7C, 0x00, 0x90])), 0xE48000, m1x0, Dyn())
check(list(fn.dyn_tables) and ('?', None, None) in fn.exit_states, 'JMP (abs,X): exits unknown')
# JMP ($9000): an absolute-indirect jump has no X offset, but its target is
# still only known at run time, so decode marks it dynamic and emit reads the
# pointer from bank 0 before dispatching.
fn = decode.decode_function(rom_with(bytes([0x6C, 0x00, 0x90])), 0xE48000, m1x0, Dyn())
check(list(fn.dyn_tables) and [i.addr for i in fn.insns] == [0xE48000] and
      ('?', None, None) in fn.exit_states, 'JMP (abs): dynamic, exits unknown')
check(0x6C in emit.implemented_opcodes(), 'JMP (abs): emitter implements opcode')
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('jmp_abs_ind', 0xE48000, ('m1x0',), 0, None, None, 'test'), fn))
check('read16_b0(0x9000)' in body and 'cpu->PC = (uint16_t)t;' in body and
      'ct_tail_rest(cpu, s0);' in body, 'JMP (abs): emitter reads bank-0 pointer and dispatches')
# JML $E50000 (not compiled): continues through the interpreter, exits unknown
fn = decode.decode_function(rom_with(bytes([0x5C, 0x00, 0x00, 0xE5])), 0xE48000, m1x0, Dyn())
check({k[0]: t for k, (t, _) in fn.interp_tails.items()} == {0xE48000: 0xE50000} and
      ('?', None, None) in fn.exit_states, 'JML to uncompiled code: interpreter tail')

# A compiled callee that comes back somewhere else (the game rewrites the
# stacked return address: RTS as a jump) must not be fatal under the frame
# scheduler: the emitter hands the rest to the executor, which continues
# from the CPU's own PB:PC. The fatal stays for the scheduler-less oracle.
class Known:
    externs = {}

    def __call__(self, site, target, st, mn):
        return {(st.m, st.x)}

    def missing(self, target, st, kind):
        return False

    def tail(self, site, target, st):
        return None


fn = decode.decode_function(rom_with(bytes([0x20, 0x00, 0x90, 0x60])), 0xE48000, m1x0, Known())
check({k[0]: t for k, (t, _) in fn.calls.items()} == {0xE48000: 0xE49000},
      f'known callee: compiled call: {fn.calls}')
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('jsr_known', 0xE48000, ('m1x0',), 0, None, None, 'test'), fn))
check('if (cpu->PC != 0x8003) { if (ct_exec_hook) { ct_tail_rest(cpu, s0); return; } '
      'cpu_check_return(cpu, 0xE48000, 0x8003); }' in body,
      f'JSR to compiled callee: non-local return hands off under the scheduler: {body}')

fn = decode.decode_function(rom_with(bytes([0x22, 0x00, 0x90, 0xE4, 0x60])), 0xE48000, m1x0,
                            Known())
body = '\n'.join(emit.emit_function(
    funcs.FuncMeta('jsl_known', 0xE48000, ('m1x0',), 0, None, None, 'test'), fn))
check('if (cpu->PC != 0x8004 || cpu->PB != 0xE4) { if (ct_exec_hook) '
      '{ ct_tail_rest(cpu, s0); return; } '
      'cpu_check_return_long(cpu, 0xE48000, 0xE48004); }' in body,
      f'JSL to compiled callee: non-local return hands off under the scheduler: {body}')

print(f"decode_calls: {'ok' if not fails else str(fails) + ' failed'}")
sys.exit(1 if fails else 0)
