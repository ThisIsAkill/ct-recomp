# Progress

Written by `tools/progress.py`. Do not edit by hand.

| Metric | Value |
|---|---|
| Routines recompiled | 15 |
| Emitted C functions (routine x entry state) | 23 |
| ROM bytes covered | 517 |
| Opcodes implemented | 54 / 256 |
| Opcode x width combinations implemented | 63 |
| Tests passing | 23 / 23 |
| Test assertions checked | 607936394 |

## Coverage by bank

| Bank | Bytes |
|---|---|
| $C1 | 517 |

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
| BattleMsg_FormatNumberDigits | $C1011F | m0x0 | 85 | c1_text |
| Battle_DivTen9499 | $C10174 | m0x0 | 53 | c1_text |
| BattleMsg_ReencodeTextBuffer | $C101A9 | m1x0 | 80 | c1_text |
| BattleUI_DrawSlotGaugeBar | $C106F0 | m1x0 | 149 | c1_ui |

## Implemented opcodes

$08 PHP impl, $0A ASL A, $10 BPL rel, $18 CLC impl, $28 PLP impl, $38 SEC impl, $48 PHA impl, $4A LSR A, $54 MVN block, $60 RTS impl, $64 STZ dp, $69 ADC imm_m, $6D ADC abs, $7B TDC impl, $80 BRA rel, $85 STA dp, $86 STX dp, $8B PHB impl, $8D STA abs, $8F STA long, $90 BCC rel, $99 STA abs_y, $9C STZ abs, $9D STA abs_x, $A0 LDY imm_x, $A2 LDX imm_x, $A5 LDA dp, $A6 LDX dp, $A8 TAY impl, $A9 LDA imm_m, $AA TAX impl, $AB PLB impl, $AD LDA abs, $AE LDX abs, $AF LDA long, $B0 BCS rel, $BD LDA abs_x, $BF LDA long_x, $C2 REP imm, $C6 DEC dp, $C8 INY impl, $C9 CMP imm_m, $CA DEX impl, $D0 BNE rel, $DA PHX impl, $E0 CPX imm_x, $E2 SEP imm, $E5 SBC dp, $E8 INX impl, $E9 SBC imm_m, $EA NOP impl, $EE INC abs, $F0 BEQ rel, $FA PLX impl

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
| interp_c1_math_mul8 | Passed |
| c1_math_mulaccum | Passed |
| interp_c1_math_mulaccum | Passed |
| c1_math_divide | Passed |
| interp_c1_math_divide | Passed |
| c1_math_shifts | Passed |
| interp_c1_math_shifts | Passed |
| c1_text_format | Passed |
| interp_c1_text_format | Passed |
| c1_text_divten | Passed |
| interp_c1_text_divten | Passed |
| c1_text_reencode | Passed |
| interp_c1_text_reencode | Passed |
| c1_ui_gauge | Passed |
| interp_c1_ui_gauge | Passed |
| diff_all | Passed |
