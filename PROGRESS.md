# Progress

Written by `tools/progress.py`. Do not edit by hand.

| Metric | Value |
|---|---|
| Routines recompiled | 205 |
| Emitted C functions (routine x entry state) | 214 |
| ROM bytes covered | 13427 |
| Opcodes implemented | 251 / 256 |
| Opcodes used by recompiled routines | 117 / 256 |
| Opcode x width combinations implemented | 446 |
| Tests passing | 23 / 23 |
| Test assertions checked | 609082337 |

## Coverage by bank

| Bank | Bytes |
|---|---|
| $C1 | 9062 |
| $C2 | 2095 |
| $C3 | 863 |
| $CD | 887 |
| $CF | 302 |
| $D1 | 218 |

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
| BattleSys_SlotPanelRefresh | $C10785 | m1x0 | 153 | c1_ui |
| BattleMenu_DrawReadyWindowEdges | $C1081E | m1x0 | 202 | c1_ui |
| BattleUI_SetPanelAttrColumn | $C108E8 | m1x0 | 65 | c1_ui |
| BattleMenu_DrawWindowEdgeStrip | $C10929 | m1x0 | 52 | c1_ui |
| Battle_SinLookup | $C101F9 | m1x0 | 41 | c1_ui |
| Calc_Delta16 | $C10222 | m1x0 | 119 | c1_ui |
| BattleUI_BuildStatusBarFrame | $C10299 | m1x0 | 782 | c1_ui |
| BattleUI_UpdateNextPcPanel | $C105A7 | m1x0 | 329 | c1_ui |
| BattleMenu_RenderItemListRows | $C1095D | m1x0 | 83 | c1_ui |
| BattleMenu_RenderItemRow | $C109B0 | m1x0 | 216 | c1_ui |
| BattleMenu_RenderTechListRows | $C10A88 | m1x0 | 75 | c1_ui |
| BattleMenu_BuildTechAvailFlags | $C10AD3 | m1x0 | 56 | c1_ui |
| BattleMenu_RenderTechRow | $C10B0B | m1x0 | 170 | c1_ui |
| CODE_JP_C10BB6 | $C10BB6 | m1x0 | 29 | c1_ui |
| BattleMenu_BlankMpCostDigits | $C10BD3 | m1x0 | 45 | c1_ui |
| CODE_JP_C10C00 | $C10C00 | m1x0 | 45 | c1_ui |
| BattleMsg_BlankLeadingZeros | $C1104E | m1x0 | 32 | c1_ui |
| BattleMenu_LoadCommandWindowMap | $C11C3A | m1x0 | 16 | c1_ui |
| BattleMenu_DrawCursorSprites | $C11115 | m1x0 | 62 | c1_ui |
| BattleMenu_CursorUp | $C11250 | m1x0 | 20 | c1_ui |
| BattleMenu_CursorDown | $C11264 | m1x0 | 23 | c1_ui |
| Battle_ZeroResultEE | $C1179C | m1x0 | 5 | c1_ui |
| BattleMenu_OpenTechList | $C112BC | m1x0 | 49 | c1_ui |
| BattleMenu_DequeueReadyBattler | $C11B67 | m1x0 | 67 | c1_ui |
| BattleMenu_RemoveBattlerFromReady | $C11BAA | m1x0 | 144 | c1_ui |
| BattleMenu_CommitAction | $C1161A | m1x0 | 308 | c1_ui |
| BattleMenu_ConsumePartnerSlot | $C1174E | m1x0 | 30 | c1_ui |
| BattleMenu_TargetNext | $C1176C | m1x0 | 26 | c1_ui |
| BattleMenu_TargetPrev | $C11786 | m1x0 | 27 | c1_ui |
| BattleMenu_TechListCancel | $C1138F | m1x0 | 30 | c1_ui |
| BattleMenu_TechListPrev | $C113AD | m1x0 | 71 | c1_ui |
| BattleMenu_TechListNext | $C113F4 | m1x0 | 73 | c1_ui |
| BattleMenu_ItemListCancel | $C114DB | m1x0 | 17 | c1_ui |
| BattleMenu_ItemCursorUp | $C114EC | m1x0 | 22 | c1_ui |
| BattleMenu_ItemCursorDown | $C11502 | m1x0 | 26 | c1_ui |
| BattleMenu_ItemListRefresh | $C1154B | m1x0 | 22 | c1_ui |
| BattleMenu_ItemListScrollUp | $C117A1 | m1x0 | 30 | c1_ui |
| BattleMenu_ItemListScrollDown | $C117BF | m1x0 | 30 | c1_ui |
| BattleUI_ClearActivePanelColumn | $C10872 | m1x0 | 118 | bankc1 |
| BattleMenu_UpdateWindows | $C10C2D | m1x0 | 654 | bankc1 |
| BattleMenu_ClearTechCursorTiles | $C10EBA | m1x0 | 39 | bankc1 |
| BattleMenu_UpdateTechMpAvail | $C10EE1 | m1x0 | 349 | bankc1 |
| BattleSys_SlotMenuReadyPredicate | $C1103E | m1x0 | 16 | bankc1 |
| BattleMenu_OpenItemList | $C112ED | m1x0 | 51 | bankc1 |
| BattleMenu_ItemListPageDown | $C1151C | m1x0 | 27 | bankc1 |
| BattleMenu_ItemListPageUp | $C11537 | m1x0 | 20 | bankc1 |
| Loc_C117B3 | $C117B3 | m1x0 | 12 | bankc1 |
| Loc_C117D1 | $C117D1 | m1x0 | 12 | bankc1 |
| BattleMenu_UpdateCursorOverlay | $C117DD | m1x0 | 828 | bankc1 |
| Text_RenderStringVec | $C20003 | m1x0 | 71 | bankc2 |
| BattleMsg_ShowFromTableCC3A09Vec | $CD0027 | m1x0 | 51 | bankcd |
| BattleMsg_ShowMsg0BIfKeyChangedVec | $CD002D | m1x0 | 59 | bankcd |
| BattleMsg_ShowMsg0CIfKeyChangedVec | $CD0030 | m1x0 | 61 | bankcd |
| BattleMsg_ClearTextBuffer | $CD0239 | m1x0 | 37 | bankcd |
| BattleMsg_RenderString | $CD025E | m1x0 | 33 | bankcd |
| Battle_MergePendingEntries1580 | $CFFAE2 | m1x0 | 131 | bankcf |
| Battle_QueueVramUpload_0CC0 | $CFFD02 | m1x0 | 52 | bankcf |
| Battle_QueueVramUpload_A6E1 | $CFFD36 | m1x0 | 52 | bankcf |
| Battle_QueueVramUpload_0E80 | $CFFD6A | m1x0 | 52 | bankcf |
| BattleFx_SetPtrA2FromTable | $CFFD9E | m1x0 | 15 | bankcf |
| BattleMenu_RefreshIfDirtyL | $C110E3 | m1x0 | 23 | bankc1 |
| BattleMenu_ProcessInput | $C11153 | m1x0 | 142 | bankc1 |
| BattleMenu_CycleActivePcPrev | $C111E1 | m1x0 | 55 | bankc1 |
| BattleMenu_CycleActivePcNext | $C11218 | m1x0 | 56 | bankc1 |
| BattleMenu_ConfirmCommand | $C1127B | m1x0 | 33 | bankc1 |
| BattleMenu_ChooseAttack | $C1129C | m1x0 | 32 | bankc1 |
| BattleMenu_TechListInput | $C11320 | m1x0 | 73 | bankc1 |
| BattleMenu_TechConfirm | $C11369 | m1x0 | 38 | bankc1 |
| BattleMenu_ItemListInput | $C1143D | m1x0 | 91 | bankc1 |
| BattleMenu_ItemConfirm | $C11498 | m1x0 | 67 | bankc1 |
| BattleMenu_TargetSelectInput | $C11561 | m1x0 | 167 | bankc1 |
| Battle_StopSfx | $C11B55 | m1x0 | 18 | bankc1 |
| BattleMenu_BuildTargetList | $C11F79 | m1x0 | 100 | bankc1 |
| Sub_C1203A | $C1203A | m1x0 | 96 | bankc1 |
| Sub_C1209A | $C1209A | m1x0 | 100 | bankc1 |
| BattleTgt_ListEnemiesSingle | $C120A9 | m1x0 | 98 | bankc1 |
| Sub_C120B6 | $C120B6 | m1x0 | 101 | bankc1 |
| Sub_C120C6 | $C120C6 | m1x0 | 101 | bankc1 |
| Sub_C120D6 | $C120D6 | m1x0 | 10 | bankc1 |
| Sub_C120E0 | $C120E0 | m1x0 | 86 | bankc1 |
| Sub_C12136 | $C12136 | m1x0 | 45 | bankc1 |
| Sub_C12163 | $C12163 | m1x0 | 47 | bankc1 |
| Sub_C12169 | $C12169 | m1x0 | 88 | bankc1 |
| Sub_C121AF | $C121AF | m1x0 | 102 | bankc1 |
| Sub_C12203 | $C12203 | m1x0 | 90 | bankc1 |
| Sub_C1224B | $C1224B | m1x0 | 38 | bankc1 |
| Sub_C1225F | $C1225F | m1x0 | 87 | bankc1 |
| Sub_C122A4 | $C122A4 | m1x0 | 65 | bankc1 |
| Sub_C122D3 | $C122D3 | m1x0 | 52 | bankc1 |
| Sub_C122F5 | $C122F5 | m1x0 | 69 | bankc1 |
| Sub_C12329 | $C12329 | m1x0 | 27 | bankc1 |
| Loc_C12332 | $C12332 | m1x0 | 114 | bankc1 |
| Loc_C123A4 | $C123A4 | m1x0 | 511 | bankc1 |
| BattleTgt_CollectTargetsInLine | $C125A3 | m1x0 | 394 | bankc1 |
| BattleTgt_CollectTargetsInRadius | $C12701 | m1x0 | 216 | bankc1 |
| BattleTgt_CompactTargetList | $C127C5 | m1x0 | 20 | bankc1 |
| BattleTgt_ClearTargetLists | $C127D9 | m1x0 | 15 | bankc1 |
| BattleTgt_CursorSeekNextValid | $C127FA | m1x0 | 26 | bankc1 |
| BattleTgt_CursorSeekPrevValid | $C12814 | m1x0 | 25 | bankc1 |
| BattleTgt_CheckListEmpty | $C1282D | m1x0 | 16 | bankc1 |
| Audio_PlayQueuedSong_Battle | $CD3B5F | m1x0 | 35 | bankcd |
| BattleSys_ServicePause | $CD3E44 | m1x0 | 49 | bankcd |
| BattleSys_EntryVec12 | $C10012 | m1x0 | 3 | bankc1 |
| BattleMenu_RefreshIfDirtyAndTick | $C110FA | m1x0 | 27 | bankc1 |
| Text_EngineTickVec | $C20009 | m1x0 | 34 | bankc2 |
| Text_DispatchState | $C2584A | m1x0 | 7 | bankc2 |
| TextRoutinesUNK200 | $C25893 | m1x0 | 2 | bankc2 |
| TextRoutinesUNK20104 | $C25895 | m1x0 | 4 | bankc2 |
| TextRoutinesUNK20507 | $C25897 | m1x0 | 25 | bankc2 |
| TextRoutinesUNK20608 | $C2589B | m1x0 | 23 | bankc2 |
| Text_DecodeLoop | $C258B2 | m1x0 | 81 | bankc2 |
| TextCode_StateWithParam | $C25945 | m1x0 | 14 | bankc2 |
| TextCode_SetState | $C2594F | m1x0 | 4 | bankc2 |
| TextCode_PrintArg8 | $C25953 | m1x0 | 178 | bankc2 |
| TextCode_PrintArg16 | $C25985 | m1x0 | 195 | bankc2 |
| TextCode_PrintArg24 | $C259C8 | m1x0 | 108 | bankc2 |
| TextCode_ArgCharName | $C25A26 | m1x0 | 38 | bankc2 |
| TextCode_EntityName | $C25A4C | m1x0 | 62 | bankc2 |
| RoutinesTextCharacter1200Tech | $C25A8E | m1x0 | 76 | bankc2 |
| RoutinesTextCharacter1201Monst | $C25AD0 | m1x0 | 54 | bankc2 |
| TextCode_ExtGlyph | $C25B06 | m1x0 | 95 | bankc2 |
| TextCode_Nop | $C25B14 | m1x0 | 1 | bankc2 |
| TextCode_PcName | $C25B15 | m1x0 | 38 | bankc2 |
| TextCode_CronoName | $C25B3B | m1x0 | 27 | bankc2 |
| TextCode_Nadia | $C25B56 | m1x0 | 26 | bankc2 |
| TextCode_PartyMemberName | $C25B70 | m1x0 | 43 | bankc2 |
| TextCode_PrintScriptValue | $C25B9B | m1x0 | 59 | bankc2 |
| TextCode_EpochName | $C25BD6 | m1x0 | 31 | bankc2 |
| Text_PlaySubstring | $C25BF5 | m1x0 | 51 | bankc2 |
| Sub_C25C30 | $C25C30 | m1x0 | 7 | bankc2 |
| Sub_C25C32 | $C25C32 | m1x0 | 5 | bankc2 |
| Sub_C25C37 | $C25C37 | m1x0 | 5 | bankc2 |
| _L5C3C | $C25C3C | m1x0 | 53 | bankc2 |
| Sub_C25C69 | $C25C69 | m1x0 | 7 | bankc2 |
| Sub_C25C6B | $C25C6B | m1x0 | 5 | bankc2 |
| Sub_C25C70 | $C25C70 | m1x0 | 5 | bankc2 |
| Sub_C25C75 | $C25C75 | m1x0 | 37 | bankc2 |
| Sub_C25CB2 | $C25CB2 | m1x0 | 7 | bankc2 |
| Sub_C25CB4 | $C25CB4 | m1x0 | 5 | bankc2 |
| Sub_C25CB9 | $C25CB9 | m1x0 | 5 | bankc2 |
| Sub_C25CBE | $C25CBE | m1x0 | 53 | bankc2 |
| Loc_C25CC0 | $C25CC0 | m0x0 | 67 | bankc2 |
| Text_StrLen10 | $C25D23 | m1x0 | 17 | bankc2 |
| Text_StrLen11 | $C25D34 | m1x0 | 17 | bankc2 |
| Text_MeasureName | $C25D45 | m1x0 | 17 | bankc2 |
| Text_SkipLeadZeros8 | $C25D56 | m1x0 | 110 | bankc2 |
| Text_SkipLeadZeros5 | $C25D91 | m1x0 | 51 | bankc2 |
| Text_SkipLeadZeros3 | $C25DAD | m1x0 | 23 | bankc2 |
| Text_EmitGlyph | $C25DC4 | m1x0 | 114 | bankc2 |
| Text_DrawGlyph2bpp | $C25E36 | m1x0 | 83 | bankc2 |
| Text_DrawGlyphRow2bpp | $C25E89 | m1x0 | 38 | bankc2 |
| PointersToUn_C25EBF | $C25EBF | m0x0 | 16 | bankc2 |
| PointersToUn_C25ECF | $C25ECF | m0x0 | 22 | bankc2 |
| PointersToUn_C25ED0 | $C25ED0 | m0x0 | 21 | bankc2 |
| PointersToUn_C25ED1 | $C25ED1 | m0x0 | 20 | bankc2 |
| PointersToUn_C25ED2 | $C25ED2 | m0x0 | 19 | bankc2 |
| PointersToUn_C25EE5 | $C25EE5 | m0x0 | 34 | bankc2 |
| PointersToUn_C25EE8 | $C25EE8 | m0x0 | 31 | bankc2 |
| PointersToUn_C25EEB | $C25EEB | m0x0 | 28 | bankc2 |
| Text_DrawGlyph4bpp | $C25F07 | m1x0 | 83 | bankc2 |
| Text_DrawGlyphRow4bpp | $C25F5A | m1x0 | 38 | bankc2 |
| PointersToUn_C25F90 | $C25F90 | m0x0 | 16 | bankc2 |
| PointersToUn_C25FA0 | $C25FA0 | m0x0 | 22 | bankc2 |
| PointersToUn_C25FA1 | $C25FA1 | m0x0 | 21 | bankc2 |
| PointersToUn_C25FA2 | $C25FA2 | m0x0 | 20 | bankc2 |
| PointersToUn_C25FA3 | $C25FA3 | m0x0 | 19 | bankc2 |
| PointersToUn_C25FB6 | $C25FB6 | m0x0 | 34 | bankc2 |
| PointersToUn_C25FB9 | $C25FB9 | m0x0 | 31 | bankc2 |
| PointersToUn_C25FBC | $C25FBC | m0x0 | 28 | bankc2 |
| Text_DivLoop16 | $C2614B | m1x0, m0x0 | 19 | bankc2 |
| Text_ToDec3 | $C2615E | m1x0 | 34 | bankc2 |
| Text_ToDec5 | $C26180 | m1x0 | 61 | bankc2 |
| Text_ToDec8 | $C261BD | m1x0 | 116 | bankc2 |
| Text_DivLoop24 | $C26231 | m1x0 | 50 | bankc2 |
| Gfx_DecompressVector | $C30002 | m1x0 | 863 | bankc3 |
| BattleSys_PerFrameServiceVec | $CD0009 | m1x0 | 56 | bankcd |
| BattleMsg_GetDurationFrames | $CD01A5 | m1x0 | 14 | bankcd |
| BattleBg_RestoreBaseGfx | $CD0453 | m1x0 | 83 | bankcd |
| BattleMsg_TickActive | $CD04DF | m1x0 | 30 | bankcd |
| BattleSys_InitColorMathFromField | $CD0CF6 | m1x0 | 50 | bankcd |
| BattleBg_CommitStagedPalette | $CD0E23 | m1x0 | 18 | bankcd |
| BattleBg_PrepOverlayCfg | $CD0E35 | m1x0 | 44 | bankcd |
| BattleBg_StageOverlayCfgA | $CD0E61 | m1x0 | 38 | bankcd |
| BattleBg_IsNormalBg | $CD0E9D | m1x0 | 32 | bankcd |
| BattleSys_WaitFrames | $CD3E3B | m1x0 | 9 | bankcd |
| BattleSys_RunFrameAndC1Tick | $CD3E75 | m1x0 | 8 | bankcd |
| BattleSys_RunFrame | $CD3E7D | m1x0 | 17 | bankcd |
| Sub_CD3ECE | $CD3ECE | m1x0 | 4 | bankcd |
| BattleSys_VramUploadChunked | $CD3ED2 | m1x0 | 204 | bankcd |
| Sub_D1ECF3 | $D1ECF3 | m1x0 | 218 | bankd1 |

## Implemented opcodes

$00 BRK imm, $01 ORA dp_x_ind, $02 COP imm, $03 ORA sr, $04 TSB dp, $05 ORA dp, $06 ASL dp, $07 ORA dp_ind_long, $08 PHP impl, $09 ORA imm_m, $0A ASL A, $0B PHD impl, $0C TSB abs, $0D ORA abs, $0E ASL abs, $0F ORA long, $10 BPL rel, $11 ORA dp_ind_y, $12 ORA dp_ind, $13 ORA sr_ind_y, $14 TRB dp, $15 ORA dp_x, $16 ASL dp_x, $17 ORA dp_ind_long_y, $18 CLC impl, $19 ORA abs_y, $1A INC A, $1B TCS impl, $1C TRB abs, $1D ORA abs_x, $1E ASL abs_x, $1F ORA long_x, $20 JSR abs, $21 AND dp_x_ind, $22 JSL long, $23 AND sr, $24 BIT dp, $25 AND dp, $26 ROL dp, $27 AND dp_ind_long, $28 PLP impl, $29 AND imm_m, $2A ROL A, $2B PLD impl, $2C BIT abs, $2D AND abs, $2E ROL abs, $2F AND long, $30 BMI rel, $31 AND dp_ind_y, $32 AND dp_ind, $33 AND sr_ind_y, $34 BIT dp_x, $35 AND dp_x, $36 ROL dp_x, $37 AND dp_ind_long_y, $38 SEC impl, $39 AND abs_y, $3A DEC A, $3B TSC impl, $3C BIT abs_x, $3D AND abs_x, $3E ROL abs_x, $3F AND long_x, $41 EOR dp_x_ind, $43 EOR sr, $45 EOR dp, $46 LSR dp, $47 EOR dp_ind_long, $48 PHA impl, $49 EOR imm_m, $4A LSR A, $4B PHK impl, $4C JMP abs, $4D EOR abs, $4E LSR abs, $4F EOR long, $50 BVC rel, $51 EOR dp_ind_y, $52 EOR dp_ind, $53 EOR sr_ind_y, $54 MVN block, $55 EOR dp_x, $56 LSR dp_x, $57 EOR dp_ind_long_y, $58 CLI impl, $59 EOR abs_y, $5A PHY impl, $5B TCD impl, $5C JML long, $5D EOR abs_x, $5E LSR abs_x, $5F EOR long_x, $60 RTS impl, $61 ADC dp_x_ind, $62 PER rlong, $63 ADC sr, $64 STZ dp, $65 ADC dp, $66 ROR dp, $67 ADC dp_ind_long, $68 PLA impl, $69 ADC imm_m, $6A ROR A, $6B RTL impl, $6D ADC abs, $6E ROR abs, $6F ADC long, $70 BVS rel, $71 ADC dp_ind_y, $72 ADC dp_ind, $73 ADC sr_ind_y, $74 STZ dp_x, $75 ADC dp_x, $76 ROR dp_x, $77 ADC dp_ind_long_y, $78 SEI impl, $79 ADC abs_y, $7A PLY impl, $7B TDC impl, $7C JMP abs_x_ind, $7D ADC abs_x, $7E ROR abs_x, $7F ADC long_x, $80 BRA rel, $81 STA dp_x_ind, $82 BRL rlong, $83 STA sr, $84 STY dp, $85 STA dp, $86 STX dp, $87 STA dp_ind_long, $88 DEY impl, $89 BIT imm_m, $8A TXA impl, $8B PHB impl, $8C STY abs, $8D STA abs, $8E STX abs, $8F STA long, $90 BCC rel, $91 STA dp_ind_y, $92 STA dp_ind, $93 STA sr_ind_y, $94 STY dp_x, $95 STA dp_x, $96 STX dp_y, $97 STA dp_ind_long_y, $98 TYA impl, $99 STA abs_y, $9A TXS impl, $9B TXY impl, $9C STZ abs, $9D STA abs_x, $9E STZ abs_x, $9F STA long_x, $A0 LDY imm_x, $A1 LDA dp_x_ind, $A2 LDX imm_x, $A3 LDA sr, $A4 LDY dp, $A5 LDA dp, $A6 LDX dp, $A7 LDA dp_ind_long, $A8 TAY impl, $A9 LDA imm_m, $AA TAX impl, $AB PLB impl, $AC LDY abs, $AD LDA abs, $AE LDX abs, $AF LDA long, $B0 BCS rel, $B1 LDA dp_ind_y, $B2 LDA dp_ind, $B3 LDA sr_ind_y, $B4 LDY dp_x, $B5 LDA dp_x, $B6 LDX dp_y, $B7 LDA dp_ind_long_y, $B8 CLV impl, $B9 LDA abs_y, $BA TSX impl, $BB TYX impl, $BC LDY abs_x, $BD LDA abs_x, $BE LDX abs_y, $BF LDA long_x, $C0 CPY imm_x, $C1 CMP dp_x_ind, $C2 REP imm, $C3 CMP sr, $C4 CPY dp, $C5 CMP dp, $C6 DEC dp, $C7 CMP dp_ind_long, $C8 INY impl, $C9 CMP imm_m, $CA DEX impl, $CB WAI impl, $CC CPY abs, $CD CMP abs, $CE DEC abs, $CF CMP long, $D0 BNE rel, $D1 CMP dp_ind_y, $D2 CMP dp_ind, $D3 CMP sr_ind_y, $D4 PEI dp, $D5 CMP dp_x, $D6 DEC dp_x, $D7 CMP dp_ind_long_y, $D8 CLD impl, $D9 CMP abs_y, $DA PHX impl, $DB STP impl, $DD CMP abs_x, $DE DEC abs_x, $DF CMP long_x, $E0 CPX imm_x, $E1 SBC dp_x_ind, $E2 SEP imm, $E3 SBC sr, $E4 CPX dp, $E5 SBC dp, $E6 INC dp, $E7 SBC dp_ind_long, $E8 INX impl, $E9 SBC imm_m, $EA NOP impl, $EB XBA impl, $EC CPX abs, $ED SBC abs, $EE INC abs, $EF SBC long, $F0 BEQ rel, $F1 SBC dp_ind_y, $F2 SBC dp_ind, $F3 SBC sr_ind_y, $F4 PEA abs, $F5 SBC dp_x, $F6 INC dp_x, $F7 SBC dp_ind_long_y, $F8 SED impl, $F9 SBC abs_y, $FA PLX impl, $FB XCE impl, $FC JSR abs_x_ind, $FD SBC abs_x, $FE INC abs_x, $FF SBC long_x

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
