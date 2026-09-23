# Progress

Written by `tools/progress.py`. Do not edit by hand.

| Metric | Value |
|---|---|
| Routines recompiled | 11 |
| Emitted C functions (routine x entry state) | 19 |
| ROM bytes covered | 150 |
| Opcodes implemented | 23 / 256 |
| Opcode x width combinations implemented | 27 |
| Tests passing | 10 / 10 |
| Test assertions checked | 303886399 |

## Coverage by bank

| Bank | Bytes |
|---|---|
| $C1 | 150 |

## Routines

| Routine | Address | Entry states | Bytes | Module |
|---|---|---|---|---|
| Battle_Mul8 | $C10089 | m1x0 | 30 | c1_math |
| Battle_MulAccum | $C100A7 | m1x0 | 48 | c1_math |
| Battle_Divide | $C100D7 | m1x0 | 54 | c1_math |
| Battle_ShiftLeft7 | $C1010D | m1x0, m0x0 | 9 | c1_math |
| Battle_ShiftLeft4 | $C10111 | m1x0, m0x0 | 5 | c1_math |
| Battle_ShiftLeft3 | $C10112 | m1x0, m0x0 | 4 | c1_math |
| Loc_C10116 | $C10116 | m1x0, m0x0 | 9 | c1_math |
| Battle_ShiftRight6 | $C10118 | m1x0, m0x0 | 7 | c1_math |
| Battle_ShiftRight5 | $C10119 | m1x0, m0x0 | 6 | c1_math |
| Battle_ShiftRight4 | $C1011A | m1x0, m0x0 | 5 | c1_math |
| Battle_ShiftRight3 | $C1011B | m1x0, m0x0 | 4 | c1_math |

## Implemented opcodes

$08 PHP impl, $0A ASL A, $28 PLP impl, $48 PHA impl, $4A LSR A, $60 RTS impl, $64 STZ dp, $6D ADC abs, $7B TDC impl, $85 STA dp, $86 STX dp, $8B PHB impl, $8D STA abs, $8F STA long, $9C STZ abs, $A5 LDA dp, $AB PLB impl, $AE LDX abs, $AF LDA long, $C2 REP imm, $E2 SEP imm, $EA NOP impl, $EE INC abs

## Tests

| Test | Result |
|---|---|
| runtime | Passed |
| runtime_fatal_open_bus | Passed |
| runtime_fatal_rom_write | Passed |
| runtime_fatal_unhooked | Passed |
| runtime_fatal_decimal | Passed |
| runtime_fatal_entry | Passed |
| c1_math_mul8 | Passed |
| c1_math_mulaccum | Passed |
| c1_math_divide | Passed |
| c1_math_shifts | Passed |
