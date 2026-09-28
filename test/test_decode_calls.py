#!/usr/bin/env python3
"""recomp/decode.py on synthetic bytes (no ROM): calls to code that isn't
compiled become interpreter calls (#30), with the continuation decoded in
the call's own M/X; decode.assumed_calls marks the calls whose M/X rest on
such an assumption until a REP/SEP grounds them; a JMP to the routine's own
entry is a loop, not a tail call; WDM is never code."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'recomp'))
import decode  # noqa: E402

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

rom = rom_with(bytes([0xEA, 0x42, 0x00, 0x60]))
try:
    decode.decode_function(rom, 0xE48000, m1x0, Resolver())
    check(False, 'WDM rejected')
except decode.DecodeError as ex:
    check('WDM' in str(ex), f'WDM rejected: {ex}')

print(f"decode_calls: {'ok' if not fails else str(fails) + ' failed'}")
sys.exit(1 if fails else 0)
