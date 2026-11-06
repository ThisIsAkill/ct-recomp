# Progress

Written by `tools/progress.py`. Do not edit by hand.

| Metric | Value |
|---|---|
| Routines recompiled | 2468 |
| Emitted C functions (routine x entry state) | 2603 |
| ROM bytes covered | 189249 |
| Functions known total (validated + pending + unresolved) | 3090 |
| Functions validated | 2468 |
| Functions unresolved (pending sync) | 622 |
| Manual roots not yet emittable | 0 |
| Opcodes implemented | 256 / 256 |
| Opcodes used by recompiled routines | 207 / 256 |
| Opcode x width combinations implemented | 449 |
| Tests passing | 74 / 75 |
| Test assertions checked | 623272191 |

## Coverage by bank

| Bank | Bytes |
|---|---|
| $00 | 8 |
| $C0 | 59526 |
| $C1 | 58057 |
| $C2 | 29826 |
| $C3 | 2356 |
| $C6 | 40 |
| $C7 | 1496 |
| $CC | 4911 |
| $CD | 6876 |
| $CE | 490 |
| $CF | 7009 |
| $D1 | 2265 |
| $FD | 15444 |
| $FF | 945 |

## Symbol sync by bank

| Bank | Validated | Unresolved | Known total |
|---|---|---|---|
| $00 | 2 | 0 | 2 |
| $C0 | 855 | 622 | 1477 |
| $C1 | 627 | 0 | 627 |
| $C2 | 565 | 0 | 565 |
| $C3 | 38 | 0 | 38 |
| $C6 | 1 | 0 | 1 |
| $C7 | 10 | 0 | 10 |
| $CC | 13 | 0 | 13 |
| $CD | 140 | 0 | 140 |
| $CE | 4 | 0 | 4 |
| $CF | 53 | 0 | 53 |
| $D1 | 18 | 0 | 18 |
| $FD | 134 | 0 | 134 |
| $FF | 8 | 0 | 8 |

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
| Battle_ShiftRight4 | $C1011A | m1x0, m0x0, m1x1 | 5 | c1_math |
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
| Text_RenderStringVec | $C20003 | m1x0, m0x0 | 3/71 | bankc2 |
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
| Audio_PlayQueuedSong_Battle | $CD3B5F | m1x0 | 18 | bankcd |
| BattleSys_ServicePause | $CD3E44 | m1x0 | 49 | bankcd |
| BattleSys_EntryVec12 | $C10012 | m1x0 | 3 | bankc1 |
| BattleMenu_RefreshIfDirtyAndTick | $C110FA | m1x0 | 27 | bankc1 |
| Text_EngineTickVec | $C20009 | m1x0 | 3 | bankc2 |
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
| Gfx_DecompressVector | $C30002 | m1x0, m0x0 | 3 | bankc3 |
| BattleSys_PerFrameServiceVec | $CD0009 | m1x0 | 3 | bankcd |
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
| Field_ColdBootInit | $C0000E | m1x0 | 250 | bankc0 |
| Field_TickCoreServices | $C000DE | m1x0 | 13 | bankc0 |
| Field_LoadLocationResources | $C000F4 | m1x0 | 39 | bankc0 |
| Sub_011B | $C0011B | m1x0 | 138 | bankc0 |
| Field_ExportBattleHandoffData | $C0038F | m1x0 | 180 | bankc0 |
| Field_ExportScreenTileProps | $C0039B | m1x0 | 127 | bankc0 |
| Field_SavePartyBattleRecords | $C0041A | m1x0 | 211 | bankc0 |
| Field_BuildEnemyBattleRecords | $C004ED | m1x0 | 106 | bankc0 |
| Field_BuildEnemyBattleRecord | $C00557 | m1x1 | 192 | bankc0 |
| Field_BuildOnscreenEnemyList | $C00617 | m1x1 | 86 | bankc0 |
| Field_RestoreObjsAfterBattle | $C00715 | m1x0 | 86 | bankc0 |
| Field_RestoreEnemyObjsAfterBattle | $C0076B | m1x1 | 129 | bankc0 |
| Obj_HideIfPcAbsent | $C007EC | m1x1 | 1866 | bankc0 |
| Field_RestorePartyPositions | $C00871 | m1x1 | 47 | bankc0 |
| Field_PlacePcObjFromSaved | $C008A0 | m1x1 | 1834 | bankc0 |
| Sub_0905 | $C00905 | m1x0, m1x1 | 19/6 | bankc0 |
| Sub_0918 | $C00918 | m1x0 | 19 | bankc0 |
| Field_CalcLocationRecordOffset | $C0092B | m1x0 | 53 | bankc0 |
| Field_LoadTilesetGfx | $C00960 | m1x0 | 231 | bankc0 |
| Field_LoadTileAssembly12 | $C009DD | m1x0 | 55 | bankc0 |
| Field_LoadTileAssemblyL3 | $C00A14 | m1x0 | 60 | bankc0 |
| Field_RebuildMapVideoState | $C00A50 | m1x0 | 175 | bankc0 |
| Field_RefreshHdmaLong | $C00AFF | m1x0 | 41 | bankc0 |
| Field_EnableNmiIrqAfterInit | $C00B28 | m1x0 | 38 | bankc0 |
| InitHW | $C00B4E | m1x0, m1x1 | 22 | bankc0 |
| Field_InstallNmiVector | $C00B64 | m1x0, m0x0, m0x1, m1x1 | 17/5/5/17 | bankc0 |
| Field_InstallIrqVector | $C00B75 | m1x0, m0x0, m0x1, m1x1 | 17 | bankc0 |
| Field_InitLocationStateVars | $C00B86 | m1x0 | 240 | bankc0 |
| DefaultHandler | $C016DC | m1x0 | 541 | bankc0 |
| Sub_18D9 | $C018D9 | m1x0 | 139 | bankc0 |
| Sub_1985 | $C01985 | m1x0 | 77 | bankc0 |
| Party_ReinitIfChanged | $C01A03 | m1x0 | 169 | bankc0 |
| Sub_1ADF | $C01ADF | m1x0 | 87 | bankc0 |
| Sub_1B36 | $C01B36 | m0x0 | 29 | bankc0 |
| Field_StartLocationMusic | $C01B53 | m1x0 | 61 | bankc0 |
| Sub_1B90 | $C01B90 | m1x0 | 23 | bankc0 |
| Sub_1BA7 | $C01BA7 | m1x0 | 25 | bankc0 |
| Sub_1F24 | $C01F24 | m1x0 | 54 | bankc0 |
| Sub_1F5A | $C01F5A | m1x0 | 45 | bankc0 |
| Dialog_FrameUpdate | $C01F87 | m1x0 | 299 | bankc0 |
| Dialog_InitWindowArea | $C020B2 | m1x0 | 64 | bankc0 |
| Dialog_RenderRequest | $C020F2 | m1x0 | 52 | bankc0 |
| Dialog_HandleRenderResult | $C02126 | m1x0 | 187 | bankc0 |
| Field_ProcessDeferredTasks | $C0274D | m1x0 | 215 | bankc0 |
| Field_FadeInAfterReload | $C02824 | m1x0 | 36 | bankc0 |
| Field_ClearOpenedTileQueue | $C028AA | m1x0 | 22 | bankc0 |
| Field_QueueOpenedTileOffset | $C028C0 | m1x0 | 33 | bankc0 |
| Field_ReapplyOpenedTileGroups | $C028E1 | m1x0 | 24 | bankc0 |
| Field_AdvanceTileGroupAt | $C028F9 | m0x0 | 254 | bankc0 |
| Dialog_BuildWindowFrameTilemap | $C029F7 | m1x0 | 385 | bankc0 |
| Dialog_BuildTextAreaTilemap | $C02B78 | m1x0 | 186 | bankc0 |
| Field_FillWramPairs | $C02C32 | m1x1 | 15 | bankc0 |
| ScrollStepAccum | $C02C41 | m1x0 | 391 | bankc0 |
| Dma7_VramCopy_Full | $C02DC8 | m1x0 | 41 | bankc0 |
| ClearRAMDMA | $C02DF1 | m1x0 | 45 | bankc0 |
| Sys_HaltWithColor | $C02E1E | m1x0 | 73 | bankc0 |
| NewGameSave_DecompressToWork | $C056D4 | m1x0 | 128 | bankc0 |
| Field_ClearSpriteUploadQueue | $C05929 | m1x0 | 51 | bankc0 |
| Evt_RunObjInit | $C0597D | m1x0 | 92 | bankc0 |
| Field_TickObjects | $C059D9 | m1x1 | 109 | bankc0 |
| Obj_CallActivateFunction | $C05AC5 | m1x1 | 90 | bankc0 |
| Obj_AbortScriptToIdle | $C05B1F | m1x1 | 68 | bankc0 |
| Obj_CmpYBottomBound | $C05B63 | m1x0 | 14 | bankc0 |
| Obj_CmpYTopBound | $C05B71 | m1x0 | 8 | bankc0 |
| Obj_CmpXRightBound | $C05B79 | m1x0 | 13 | bankc0 |
| Obj_CmpXLeftBound | $C05B86 | m1x0 | 7 | bankc0 |
| Field_SanityAsserts | $C05CC7 | m1x0 | 110 | bankc0 |
| Field_LoadTilesetChunkTo3000 | $C06D2F | m1x0 | 54 | bankc0 |
| Field_LoadLocationL3Tileset | $C06DCF | m1x0 | 88 | bankc0 |
| Field_UploadWindowFrameGfx | $C06E5C | m1x0, m0x0 | 68 | bankc0 |
| Vblank_UploadSpriteTileCache | $C06EB0 | m1x0 | 27 | bankc0 |
| Dialog_UploadTextAreaTilemap | $C06EF1 | m1x0 | 27 | bankc0 |
| Field_UploadFixedUiTiles | $C06F0C | m1x0 | 82 | bankc0 |
| Dialog_UploadBlankTextTile | $C06F5E | m1x0 | 27 | bankc0 |
| Field_ResetSpriteTileSlots | $C06F79 | m1x0 | 33 | bankc0 |
| Obj_ReleaseSpriteTileSlot | $C07056 | m1x1 | 46 | bankc0 |
| Field_LoadLocationBgPalettes | $C07084 | m1x0 | 101 | bankc0 |
| Field_LoadWindowPaletteAndMirror | $C070E9 | m1x0 | 69 | bankc0 |
| Field_PaletteNopStub | $C07154 | m1x0 | 1 | bankc0 |
| Field_ResetSpritePaletteSlots | $C07155 | m1x0 | 27 | bankc0 |
| Obj_AssignPaletteSlot1to3 | $C072B4 | m1x1 | 152 | bankc0 |
| Obj_ReclaimSpritePaletteSlot | $C0734C | m1x1 | 77 | bankc0 |
| Camera_InitMapBoundsAndMasks | $C07399 | m1x0 | 269 | bankc0 |
| Camera_InitScrollWindow | $C074A6 | m1x0 | 465 | bankc0 |
| Camera_ResetL1ScrollDelta | $C074D4 | m1x0 | 20 | bankc0 |
| Camera_ResetL2ScrollDelta | $C074E8 | m1x0 | 22 | bankc0 |
| Camera_ResetL3ScrollDelta | $C074F7 | m1x0 | 22 | bankc0 |
| Camera_ClampScrollX | $C07506 | m1x0 | 77 | bankc0 |
| Camera_ClampScrollY | $C07553 | m1x0 | 77 | bankc0 |
| Loc_C075A0 | $C075A0 | m1x0 | 73 | bankc0 |
| Field_RedrawL1TilemapBuf | $C075E9 | m1x0 | 41 | bankc0 |
| Field_DrawL1TileRowStrip | $C07612 | m1x0 | 466 | bankc0 |
| Field_DrawL1TileColStrip | $C077E4 | m1x0 | 264 | bankc0 |
| Field_RedrawL2TilemapBuf | $C078EC | m1x0 | 227 | bankc0 |
| Field_DrawL2TileRowStrip | $C079CF | m1x0 | 474 | bankc0 |
| Field_DrawL2TileColStrip | $C07BA9 | m1x0 | 268 | bankc0 |
| Field_RedrawL3TilemapBuf | $C07CB5 | m1x0 | 177 | bankc0 |
| Field_DrawL3TileRowStrip | $C07D66 | m1x0 | 250 | bankc0 |
| Field_DrawL3TileColStrip | $C07E60 | m1x0 | 248 | bankc0 |
| Field_UploadTilemapC800 | $C07F62 | m1x0 | 62 | bankc0 |
| Field_UploadTilemapC800Big | $C07F77 | m1x0 | 64 | bankc0 |
| Field_ClearPage1D00 | $C07F7E | m1x0 | 28 | bankc0 |
| Field_CalcScrollEdgeVramAddrs | $C07F9A | m1x0 | 419 | bankc0 |
| Field_CalcRowStripVramSplit | $C0813D | m0x0 | 100 | bankc0 |
| Field_CalcColStripAddrs | $C081A1 | m0x0 | 53 | bankc0 |
| Field_CalcColStripAddrsAlt | $C081D6 | m0x0 | 66 | bankc0 |
| Field_CalcTilemapAddrWrap | $C08218 | m0x0 | 43 | bankc0 |
| Field_BuildRowStripBottomL1 | $C08243 | m1x0 | 32 | bankc0 |
| Field_BuildRowStripBottomL2 | $C08263 | m1x0 | 34 | bankc0 |
| Field_BuildRowStripBottomL2Ind | $C08285 | m1x0 | 32 | bankc0 |
| Field_BuildRowStripBottomL3 | $C082A5 | m1x0 | 32 | bankc0 |
| Field_BuildRowStripTopL1 | $C082C5 | m1x0 | 32 | bankc0 |
| Field_BuildRowStripTopL2 | $C082E5 | m1x0 | 32 | bankc0 |
| Field_BuildRowStripTopL2Ind | $C08305 | m1x0 | 32 | bankc0 |
| Field_BuildRowStripTopL3 | $C08325 | m1x0 | 32 | bankc0 |
| Field_BuildColStripRightL1 | $C08345 | m1x0 | 32 | bankc0 |
| Field_BuildColStripRightL2 | $C08365 | m1x0 | 32 | bankc0 |
| Field_BuildColStripRightL2Ind | $C08385 | m1x0 | 32 | bankc0 |
| Field_BuildColStripRightL3 | $C083A5 | m1x0 | 32 | bankc0 |
| Field_BuildColStripLeftL1 | $C083C5 | m1x0 | 32 | bankc0 |
| Field_BuildColStripLeftL2 | $C083E5 | m1x0 | 32 | bankc0 |
| Field_BuildColStripLeftL2Ind | $C08405 | m1x0 | 32 | bankc0 |
| Field_BuildColStripLeftL3 | $C08425 | m1x0 | 32 | bankc0 |
| Field_BuildRightColStripsImmediate | $C087F1 | m1x0 | 45 | bankc0 |
| Camera_RecenterProcess | $C0885A | m1x0 | 139 | bankc0 |
| Camera_LoadMoveVel | $C088E5 | m1x0 | 9 | bankc0 |
| Camera_MoveDispatch | $C08A6D | m1x1 | 2136 | bankc0 |
| Camera_TryStepToTarget | $C08A9E | m0x1, m0x0 | 23 | bankc0 |
| Camera_CommitFrameDeltas | $C09175 | m1x0 | 55 | bankc0 |
| Camera_SeekTargetTile | $C091AC | m1x0 | 565 | bankc0 |
| Camera_ApplyScrollSteps | $C093E1 | m1x0 | 525 | bankc0 |
| Camera_ScrollStepRightL1 | $C0944B | m1x0 | 50 | bankc0 |
| Camera_ScrollStepLeftL1 | $C0947D | m1x0 | 48 | bankc0 |
| Camera_ScrollStepDownL1 | $C094AD | m1x0 | 50 | bankc0 |
| Camera_ScrollStepUpL1 | $C094DF | m1x0 | 48 | bankc0 |
| Camera_ScrollStepRightL2 | $C0950F | m1x0 | 54 | bankc0 |
| Camera_ScrollStepLeftL2 | $C09545 | m1x0 | 52 | bankc0 |
| Camera_ScrollStepDownL2 | $C09579 | m1x0 | 54 | bankc0 |
| Camera_ScrollStepUpL2 | $C095AF | m1x0 | 52 | bankc0 |
| Camera_ScrollStepRightL2Ind | $C095E3 | m1x0 | 54 | bankc0 |
| Camera_ScrollStepLeftL2Ind | $C09619 | m1x0 | 52 | bankc0 |
| Camera_ScrollStepDownL2Ind | $C0964D | m1x0 | 54 | bankc0 |
| Camera_ScrollStepUpL2Ind | $C09683 | m1x0 | 52 | bankc0 |
| Camera_ScrollStepRightL3 | $C096B7 | m1x0 | 54 | bankc0 |
| Camera_ScrollStepLeftL3 | $C096ED | m1x0 | 52 | bankc0 |
| Camera_ScrollStepDownL3 | $C09721 | m1x0 | 62 | bankc0 |
| Camera_ScrollStepUpL3 | $C0975F | m1x0 | 60 | bankc0 |
| Field_BuildRightColStrips | $C0979B | m1x0 | 49 | bankc0 |
| Field_BuildRightColStripsInd | $C097CC | m1x0 | 49 | bankc0 |
| Field_BuildLeftColStrips | $C097FD | m1x0 | 49 | bankc0 |
| Field_BuildLeftColStripsInd | $C0982E | m1x0 | 49 | bankc0 |
| Field_BuildBottomRowStrips | $C0985F | m1x0 | 49 | bankc0 |
| Field_BuildBottomRowStripsInd | $C09890 | m1x0 | 49 | bankc0 |
| Field_BuildTopRowStrips | $C098C1 | m1x0 | 49 | bankc0 |
| Field_BuildTopRowStripsInd | $C098F2 | m1x0 | 49 | bankc0 |
| Obj_FindAtPosition | $C09923 | m1x1, m1x0 | 187 | bankc0 |
| Camera_ClampVelToMapEdges | $C099DE | m1x0 | 65 | bankc0 |
| Camera_CheckBottomEdgeLimit | $C09A1F | m1x0 | 30 | bankc0 |
| Camera_CheckTopEdgeLimit | $C09A3D | m1x0 | 35 | bankc0 |
| Camera_CheckRightEdgeLimit | $C09A60 | m1x0 | 30 | bankc0 |
| Camera_CheckLeftEdgeLimit | $C09A7E | m1x0 | 35 | bankc0 |
| Camera_CheckZoneTable | $C09AA1 | m1x1, m1x0 | 39 | bankc0 |
| Player_TilePropsLookup | $C09AC8 | m1x0, m1x1 | 33 | bankc0 |
| Camera_UpdateScroll | $C09AD3 | m1x0, m1x1 | 290 | bankc0 |
| Camera_CheckZoneMatch | $C09C37 | m1x1, m1x0 | 37 | bankc0 |
| Camera_ApplyVelocity | $C09C5C | m0x1, m1x0, m1x1 | 359 | bankc0 |
| Map_LoadAndApplyConfig | $C0A33B | m1x0 | 461 | bankc0 |
| CODE_FN_C0A508 | $C0A508 | m1x0 | 1 | bankc0 |
| Field_WriteScreenDesignation | $C0A509 | m1x0 | 24 | bankc0 |
| Map_BuildExitGrid | $C0A66B | m1x0 | 167 | bankc0 |
| Map_BuildTreasureGrid | $C0A712 | m1x0 | 170 | bankc0 |
| Map_IsTreasureOpened | $C0A7BC | m1x0 | 45 | bankc0 |
| Field_ClearShadowOam | $C0A7E9 | m1x0 | 39 | bankc0 |
| Obj_RefreshVisibilityWindow | $C0A947 | m1x1 | 67 | bankc0 |
| Obj_WriteOamEntry | $C0A98A | m1x1 | 67 | bankc0 |
| Obj_YSortListInsert | $C0A9CD | m1x1, m0x1 | 58 | bankc0 |
| Obj_ApplyMoveVelocity | $C0AA07 | m1x1 | 246 | bankc0 |
| Obj_ApplyMoveVelocityLinear | $C0AAFD | m1x1 | 126 | bankc0 |
| Obj_ComputeScreenPos | $C0AB45 | m1x1, m0x1 | 93 | bankc0 |
| Sub_B192 | $C0B192 | m1x0 | 61 | bankc0 |
| Vblank_UploadSpriteTileQueue | $C0B1B2 | m1x0 | 82 | bankc0 |
| Field_ClearOamShadow | $C0B204 | m1x0 | 94 | bankc0 |
| Field_HideReservedOamSprites | $C0B262 | m1x0, m1x1 | 15 | bankc0 |
| PostVBlank | $C0B271 | m1x0 | 152 | bankc0 |
| Sub_B309 | $C0B309 | m1x0 | 1016 | bankc0 |
| Sub_B701 | $C0B701 | m1x0 | 727 | bankc0 |
| Sub_B788 | $C0B788 | m1x0 | 322 | bankc0 |
| Sub_B8CA | $C0B8CA | m1x0 | 411 | bankc0 |
| Sub_BA65 | $C0BA65 | m1x0 | 631 | bankc0 |
| Sub_BCDC | $C0BCDC | m1x0 | 790 | bankc0 |
| Sub_BFF2 | $C0BFF2 | m1x0 | 717 | bankc0 |
| Sub_C2BF | $C0C2BF | m1x0 | 1064 | bankc0 |
| Sub_C6E7 | $C0C6E7 | m0x0 | 83 | bankc0 |
| Obj_AnimTickAndQueue | $C0C98A | m1x1 | 236 | bankc0 |
| Field_ProcessAnimQueue | $C0CA76 | m1x1, m0x1 | 99 | bankc0 |
| Obj_BuildSpriteFrameStep | $C0CAD9 | m1x1 | 3818 | bankc0 |
| Sub_CB3A | $C0CB3A | m1x0, m1x1 | 162 | bankc0 |
| Sub_E12A | $C0E12A | m1x0, m1x1 | 1034 | bankc0 |
| Sub_E534 | $C0E534 | m0x0 | 339 | bankc0 |
| Sub_E687 | $C0E687 | m0x0 | 686 | bankc0 |
| Sub_E935 | $C0E935 | m1x0 | 29 | bankc0 |
| Sub_E952 | $C0E952 | m1x0, m1x1 | 33/40 | bankc0 |
| Sub_E97A | $C0E97A | m1x0, m1x1 | 48 | bankc0 |
| Sub_E9AA | $C0E9AA | m1x0, m1x1 | 85/56 | bankc0 |
| Sub_E9E2 | $C0E9E2 | m1x0 | 29 | bankc0 |
| Sub_E9FF | $C0E9FF | m1x0 | 32 | bankc0 |
| Sub_EA1F | $C0EA1F | m1x0 | 35 | bankc0 |
| Obj_ReleaseVramCells | $C0EA42 | m1x1 | 33 | bankc0 |
| Sub_EC60 | $C0EC60 | m1x0 | 23 | bankc0 |
| Map_BuildTilePropGrid | $C0A521 | m1x0 | 330 | bankc0 |
| Field_CopyMapRectLayers | $C0AF4E | m1x0 | 328 | bankc0 |
| Field_CopyMapRectMVN | $C0B096 | m1x0 | 80 | bankc0 |
| ReentryVectors | $C00000 | m1x0 | 127 | bankc0 |
| _C0L27EB | $C027EB | m1x0 | 57 | bankc0 |
| Field_RebuildScreenAndSettleFull | $C02848 | m1x0 | 36 | bankc0 |
| Field_RebuildScreen | $C0286C | m1x0 | 62 | bankc0 |
| EventCmd_PlayAnimLoop | $C02E67 | m1x0 | 32 | bankc0 |
| EventCmd_PlayAnim0 | $C02E87 | m1x0 | 30 | bankc0 |
| EventCmd_PlayAnim1 | $C02E8B | m1x0 | 30 | bankc0 |
| EventCmd_ResetAnim | $C02E8F | m1x0 | 30 | bankc0 |
| EventCmd_OpAB | $C02E9A | m1x0 | 103 | bankc0 |
| EventCmd_OpB7 | $C02F01 | m1x0 | 112 | bankc0 |
| EventCmd_SetAnimFrame | $C02F71 | m1x0 | 37 | bankc0 |
| EventCmd_Wait | $C02F96 | m1x0 | 48 | bankc0 |
| EventCmd_OpB9 | $C02FC6 | m1x0 | 45 | bankc0 |
| EventCmd_OpBA | $C02FCA | m1x0 | 45 | bankc0 |
| EventCmd_OpBD | $C02FCE | m1x0 | 45 | bankc0 |
| EventCmd_OpBC | $C02FD2 | m1x0 | 43 | bankc0 |
| EventCmd_OpAF | $C02FFD | m1x0 | 5 | bankc0 |
| EventCmd_ActAsPartyMember | $C03002 | m1x0 | 91 | bankc0 |
| Obj_StepAlignToTileCenter | $C0305D | m1x0 | 86 | bankc0 |
| Obj_StepTowardTargetTile | $C030B3 | m1x0 | 161 | bankc0 |
| Obj_CallTouchFunction | $C03154 | m1x1 | 93 | bankc0 |
| Obj_TryPushBlocker | $C031B1 | m1x1 | 187 | bankc0 |
| EventCmd_OpD9 | $C0326C | m1x0 | 494 | bankc0 |
| EventCmd_OpDA | $C0345A | m1x0 | 229 | bankc0 |
| EventCmd_OpB1 | $C0353F | m1x0 | 4 | bankc0 |
| EventCmd_OpB2 | $C03543 | m1x0 | 3 | bankc0 |
| EventCmd_OpB5 | $C03546 | m1x0 | 9 | bankc0 |
| EventCmd_OpB6 | $C0354F | m1x0 | 8 | bankc0 |
| EventCmd_SetDialogTable | $C03557 | m1x0 | 25 | bankc0 |
| EventCmd_TextboxAuto | $C03570 | m1x0 | 142 | bankc0 |
| EventCmd_TextboxTop | $C035B5 | m1x0 | 144 | bankc0 |
| EventCmd_TextboxBottom | $C035FC | m1x0 | 144 | bankc0 |
| EventCmd_OpC0 | $C03643 | m1x0 | 176 | bankc0 |
| EventCmd_OpC3 | $C036AA | m1x0 | 178 | bankc0 |
| EventCmd_OpC4 | $C036DD | m1x0 | 179 | bankc0 |
| EventCmd_SetPalette | $C03711 | m1x0 | 70 | bankc0 |
| EventCmd_OpenNameEntry | $C03757 | m1x0 | 41 | bankc0 |
| EventCmd_IfHasItem | $C03780 | m1x0 | 49 | bankc0 |
| EventCmd_AddItem | $C037B1 | m1x0 | 22 | bankc0 |
| EventCmd_OpC7 | $C037C7 | m1x0 | 39 | bankc0 |
| EventCmd_RemoveItem | $C037E3 | m1x0 | 22 | bankc0 |
| EventCmd_OpD7 | $C037F9 | m1x0 | 46 | bankc0 |
| EventCmd_IfGoldGte | $C03827 | m1x0 | 52 | bankc0 |
| EventCmd_AddGold | $C0385B | m1x0 | 25 | bankc0 |
| EventCmd_RemoveGold | $C03874 | m1x0 | 25 | bankc0 |
| EventCmd_PartyReserveCheck | $C0388D | m1x0 | 49 | bankc0 |
| EventCmd_OpD0 | $C038BE | m1x0 | 21 | bankc0 |
| EventCmd_PartyMarkInactive | $C038D3 | m1x0 | 21 | bankc0 |
| EventCmd_IfActivePartyMember | $C038E8 | m1x0 | 49 | bankc0 |
| EventCmd_AddPartyMember | $C03919 | m1x0 | 53 | bankc0 |
| EventCmd_RemovePartyMember | $C0394E | m1x0 | 147 | bankc0 |
| EventCommandD4_39E1 | $C039E1 | m1x0 | 19 | bankc0 |
| Field_CopySpritePalRow7To6 | $C039F4 | m1x0 | 19 | bankc0 |
| EventCmd_OpD6 | $C03A07 | m1x0 | 97 | bankc0 |
| EventCmd_OpD5 | $C03A68 | m1x0 | 27 | bankc0 |
| EventCmd_StartBattle | $C03A83 | m1x0 | 52 | bankc0 |
| EventCmd_ChangeLocation | $C03AB7 | m1x0 | 58 | bankc0 |
| EventCmd_OpDC | $C03AF1 | m1x0 | 42 | bankc0 |
| EventCmd_OpDD | $C03B1B | m1x0 | 42 | bankc0 |
| EventCmd_OpDE | $C03B45 | m1x0 | 48 | bankc0 |
| EventCmd_OpE1 | $C03B4B | m1x0 | 204 | bankc0 |
| EventCmd_OpDF | $C03B98 | m1x0 | 210 | bankc0 |
| EventCmd_ChangeLocationFromMem | $C03B9E | m1x0 | 103 | bankc0 |
| EventCmd_SetExploreMode | $C03C05 | m1x0 | 12 | bankc0 |
| EventCmd_PlaySound | $C03C11 | m1x0 | 30 | bankc0 |
| EventCmd_PlayMusic | $C03C2F | m1x0 | 29 | bankc0 |
| EventCmd_VolumeFade | $C03C4C | m1x0 | 76 | bankc0 |
| EventCmd_SoundCommand | $C03C98 | m1x0 | 36 | bankc0 |
| EventCmd_WaitSilence | $C03CBC | m1x0 | 9 | bankc0 |
| EventCmd_WaitMusicEnd | $C03CC5 | m1x0 | 11 | bankc0 |
| EventCmd_OpF0 | $C03CD0 | m1x0 | 45 | bankc0 |
| EventCmd_OpF2 | $C03CFD | m1x0 | 10 | bankc0 |
| EventCmd_OpF1 | $C03D07 | m1x0 | 115 | bankc0 |
| EventCmd_OpF3 | $C03D7A | m1x0 | 10 | bankc0 |
| EventCmd_ShakeScreen | $C03D84 | m1x0 | 19 | bankc0 |
| EventCmd_CopyTiles | $C03D97 | m1x0 | 46 | bankc0 |
| EventCmd_CopyTiles2 | $C03DC5 | m1x0 | 47 | bankc0 |
| EventCmd_ScrollLayers | $C03DF4 | m1x0 | 60 | bankc0 |
| EventCmd_ScrollScreen | $C03E30 | m1x0 | 44 | bankc0 |
| EventCmd_PartyRefreshBoth | $C03E5C | m1x0 | 18 | bankc0 |
| EventCmd_PartyRefreshOp6 | $C03E6E | m1x0 | 14 | bankc0 |
| EventCmd_PartyRefreshOp7 | $C03E75 | m1x0 | 14 | bankc0 |
| Loc_C03E7C | $C03E7C | m1x0 | 15 | bankc0 |
| EventCmd_Op29 | $C03E84 | m1x0 | 82 | bankc0 |
| EventCmd_DeferTask04 | $C03ED6 | m1x0 | 8 | bankc0 |
| EventCmd_DeferTask08 | $C03EDE | m1x0 | 10 | bankc0 |
| EventCmd_DeferTask10 | $C03EE2 | m1x0 | 10 | bankc0 |
| EventCmd_Op2C | $C03EE6 | m1x0 | 44 | bankc0 |
| EventCmd_Op2F | $C03F12 | m1x0 | 20 | bankc0 |
| EventCmd_DrawGeometry | $C03F26 | m1x0 | 137 | bankc0 |
| EventCmd_Mode7Scene | $C03FAF | m1x0 | 95 | bankc0 |
| PointersToEv_C04036 | $C04036 | m1x0 | 31 | bankc0 |
| PointersToEv_C04055 | $C04055 | m1x0 | 8 | bankc0 |
| PointersToEv_C0405D | $C0405D | m1x0 | 86 | bankc0 |
| SubroutinesEventCommandFF_40B3 | $C040B3 | m1x0 | 60 | bankc0 |
| PointersToEv_C040EF | $C040EF | m1x0 | 86 | bankc0 |
| SubroutinesEventCommandFF_4145 | $C04145 | m1x0 | 60 | bankc0 |
| _C0L4181 | $C04181 | m1x0 | 16 | bankc0 |
| SubroutinesEventCommandFF_4191 | $C04191 | m1x0 | 31 | bankc0 |
| SubroutinesEventCommandFF_41B0 | $C041B0 | m1x0 | 10 | bankc0 |
| SubroutinesEventCommandFF_41B4 | $C041B4 | m1x0 | 10 | bankc0 |
| SubroutinesEventCommandFF_41B8 | $C041B8 | m1x0 | 8 | bankc0 |
| SubroutinesEventCommandFF_41C0 | $C041C0 | m1x0 | 10 | bankc0 |
| SubroutinesEventCommandFF_41C4 | $C041C4 | m1x0 | 10 | bankc0 |
| SubroutinesEventCommandFF_41C8 | $C041C8 | m1x0 | 20 | bankc0 |
| SubroutinesEventCommandFF_41DC | $C041DC | m1x0 | 8 | bankc0 |
| EventCmd_LoadCrono | $C041E4 | m1x0 | 536 | bankc0 |
| EventCmd_LoadMarle | $C041EC | m1x0 | 536 | bankc0 |
| EventCmd_LoadLucca | $C041F4 | m1x0 | 536 | bankc0 |
| EventCmd_LoadFrog | $C041FC | m1x0 | 536 | bankc0 |
| EventCmd_LoadRobo | $C04204 | m1x0 | 536 | bankc0 |
| EventCmd_LoadAyla | $C0420C | m1x0 | 536 | bankc0 |
| EventCmd_LoadMagus | $C04214 | m1x0 | 534 | bankc0 |
| EventCmd_LoadPcIfInParty | $C0421E | m1x0 | 600 | bankc0 |
| EventCmd_LoadPc | $C04476 | m1x0 | 225 | bankc0 |
| Obj_LoadPcSpriteRecord | $C0448D | m1x0 | 202 | bankc0 |
| Obj_InitSlotFromScriptId | $C04557 | m1x0 | 57 | bankc0 |
| EventCmd_LoadNpc | $C04590 | m1x0 | 408 | bankc0 |
| EventCmd_LoadEnemy | $C046DF | m1x0 | 465 | bankc0 |
| EventCmd_SetSolidProps | $C04867 | m1x0 | 15 | bankc0 |
| EventCmd_SetObjectGfx | $C04876 | m1x0 | 28 | bankc0 |
| EventCmd_PaletteCommand | $C04892 | m1x0 | 444 | bankc0 |
| EventCmd_Op2E | $C04A4E | m1x0 | 222 | bankc0 |
| Field_FindFreePaletteTaskSlot | $C04B2C | m1x0 | 29 | bankc0 |
| EventCmd_SetSpeed | $C04B49 | m1x0 | 15 | bankc0 |
| EventCmd_SetSpeedFromMem | $C04B58 | m1x0 | 28 | bankc0 |
| EventCmd_SetObjectCoordTiles | $C04B74 | m1x0 | 79 | bankc0 |
| EventCmd_SetCoordsFromMem | $C04BC3 | m1x0 | 100 | bankc0 |
| EventCmd_SetObjectCoordPixels | $C04C27 | m1x0 | 77 | bankc0 |
| EventCmd_SetSpritePriority | $C04C74 | m1x0 | 97 | bankc0 |
| EventCmd_ShowObject | $C04CD5 | m1x0 | 11 | bankc0 |
| EventCmd_HideObject | $C04CE0 | m1x0 | 13 | bankc0 |
| EventCmd_Op7E | $C04CE6 | m1x0 | 13 | bankc0 |
| EventCmd_ShowObjectN | $C04CEC | m1x0 | 20 | bankc0 |
| EventCmd_HideObjectN | $C04CF9 | m1x0 | 20 | bankc0 |
| EventCmd_NpcJump | $C04D06 | m1x0 | 287 | bankc0 |
| EventCmd_Op7B | $C04E25 | m1x0 | 78 | bankc0 |
| EventCmd_Op92 | $C04E73 | m1x0 | 84 | bankc0 |
| EventCmd_Op9C | $C04EC7 | m1x0 | 67 | bankc0 |
| EventCmd_Op9D | $C04F0A | m1x0 | 87 | bankc0 |
| EventCmd_MoveToCoords | $C04F61 | m1x0 | 137 | bankc0 |
| EventCmd_Op9A | $C04FEA | m1x0 | 154 | bankc0 |
| EventCmd_MoveToCoords16 | $C05084 | m1x0 | 165 | bankc0 |
| EventCmd_AnimMoveToCoords | $C05129 | m1x0 | 104 | bankc0 |
| EventCmd_AnimMoveToCoords16 | $C05191 | m1x0 | 132 | bankc0 |
| EventCmd_FollowObject | $C05215 | m1x0 | 171 | bankc0 |
| EventCmd_Op9E | $C052C0 | m1x0 | 138 | bankc0 |
| EventCmd_MoveTowardObject | $C0534A | m1x0 | 198 | bankc0 |
| EventCmd_FollowPc | $C05410 | m1x0 | 177 | bankc0 |
| EventCmd_Op8F | $C05429 | m1x0 | 198 | bankc0 |
| EventCmd_Op9F | $C054F5 | m1x0 | 144 | bankc0 |
| EventCmd_MoveTowardPc | $C0550E | m1x0 | 204 | bankc0 |
| EventCmd_FaceUp | $C05535 | m1x0 | 16 | bankc0 |
| EventCmd_FaceDown | $C05539 | m1x0 | 16 | bankc0 |
| EventCmd_FaceLeft | $C0553D | m1x0 | 16 | bankc0 |
| EventCmd_FaceRight | $C05541 | m1x0 | 16 | bankc0 |
| EventCmd_SetFacing | $C05545 | m1x0 | 18 | bankc0 |
| EventCmd_SetFacingFromMem | $C05557 | m1x0 | 33 | bankc0 |
| EventCmd_ObjectFaceUp | $C0556C | m1x0 | 23 | bankc0 |
| EventCmd_ObjectFaceDown | $C05579 | m1x0 | 23 | bankc0 |
| EventCmd_ObjectFaceLeft | $C05586 | m1x0 | 23 | bankc0 |
| EventCmd_ObjectFaceRight | $C05593 | m1x0 | 23 | bankc0 |
| EventCmd_FaceTowardObject | $C055A0 | m1x0 | 101 | bankc0 |
| EventCmd_FaceTowardPc | $C05605 | m1x0 | 107 | bankc0 |
| Obj_CheckAnimDone | $C05614 | m1x0 | 119 | bankc0 |
| Obj_ResetAnimToIdle | $C0568B | m1x0 | 27 | bankc0 |
| Field_LocationInit | $C056A6 | m1x0 | 46 | bankc0 |
| EventScript_InitBlock | $C05709 | m1x0 | 646 | bankc0 |
| Sub_595C | $C0595C | m1x0 | 33 | bankc0 |
| Obj_TestBlockerSlotEB | $C05B8D | m1x1 | 8 | bankc0 |
| Obj_ScanObjAheadOfFacing | $C05B95 | m1x0 | 93 | bankc0 |
| Sub_C05BFA | $C05BFA | m1x1 | 42 | bankc0 |
| Sub_C05C20 | $C05C20 | m1x1 | 39 | bankc0 |
| Sub_C05C47 | $C05C47 | m1x1 | 42 | bankc0 |
| Sub_C05C6D | $C05C6D | m1x1 | 39 | bankc0 |
| Field_LookupOrRegisterId0920 | $C05C90 | m1x0 | 128 | bankc0 |
| EventCmd_Nop | $C05F6E | m1x0 | 79 | bankc0 |
| EventCmd_Return | $C05F74 | m1x0 | 66 | bankc0 |
| EventCmd_CallObjectFn | $C05FB6 | m1x0 | 200 | bankc0 |
| EventCmd_CallObjectFnSync | $C0607E | m1x0 | 139 | bankc0 |
| EventCmd_CallObjectFnWait | $C06109 | m1x0 | 224 | bankc0 |
| EventCmd_CallPcFn | $C061E9 | m1x0 | 213 | bankc0 |
| EventCmd_CallPcFnSync | $C061FE | m1x0 | 153 | bankc0 |
| EventCmd_CallPcFnWait | $C06214 | m1x0 | 241 | bankc0 |
| EventCmd_SetSelfCallLock | $C06240 | m1x0 | 11 | bankc0 |
| EventCmd_ClearSelfCallLock | $C0624B | m1x0 | 13 | bankc0 |
| EventCmd_RemoveObject | $C06254 | m1x0 | 25 | bankc0 |
| EventCmd_DisableProcessing | $C06269 | m1x0 | 29 | bankc0 |
| EventCmd_EnableProcessing | $C06282 | m1x0 | 23 | bankc0 |
| EventCmd_SetMoveProps | $C06295 | m1x0 | 19 | bankc0 |
| EventCmd_SetMoveProps2 | $C062A4 | m1x0 | 17 | bankc0 |
| EventCmd_GotoForward | $C062B5 | m1x0 | 22 | bankc0 |
| EventCmd_GotoBack | $C062CB | m1x0 | 24 | bankc0 |
| EventCmd_IfMemCmpImm8 | $C062DE | m1x0 | 77 | bankc0 |
| EventCmd_IfMemCmpImm16 | $C06313 | m1x0 | 78 | bankc0 |
| EventCmd_IfMemCmpMem8 | $C06361 | m1x0 | 91 | bankc0 |
| EventCmd_IfMemCmpMem16 | $C063A4 | m1x0 | 90 | bankc0 |
| EventCmd_IfEventMem | $C063E6 | m1x0 | 116 | bankc0 |
| EventCmd_IfStorylineLt | $C06442 | m1x0 | 37 | bankc0 |
| EventCommand16OperatorRoutines | $C06487 | m1x1 | 10 | bankc0 |
| EventCommand16OperatorRoutines_648F | $C0648F | m1x1 | 10 | bankc0 |
| EventCommand16OperatorRoutines_6497 | $C06497 | m1x1 | 12 | bankc0 |
| EventCommand16OperatorRoutines_64A1 | $C064A1 | m1x1 | 10 | bankc0 |
| EventCommand16OperatorRoutines_64A9 | $C064A9 | m1x1 | 12 | bankc0 |
| EventCommand16OperatorRoutines_64B5 | $C064B5 | m1x1 | 12 | bankc0 |
| EventCommand16OperatorRoutines_64BF | $C064BF | m1x1 | 10 | bankc0 |
| EventCommand16OperatorRoutines_64C7 | $C064C7 | m1x1 | 10 | bankc0 |
| Sub_C064CF | $C064CF | m1x1 | 16 | bankc0 |
| Sub_C064DB | $C064DB | m1x1 | 16 | bankc0 |
| Sub_C064E7 | $C064E7 | m1x1 | 18 | bankc0 |
| Sub_C064F5 | $C064F5 | m1x1 | 18 | bankc0 |
| Sub_C06503 | $C06503 | m1x1 | 18 | bankc0 |
| Sub_C06511 | $C06511 | m1x1 | 18 | bankc0 |
| Sub_C06523 | $C06523 | m1x1 | 16 | bankc0 |
| Sub_C0652F | $C0652F | m1x1 | 16 | bankc0 |
| EventCmd_MemToResult | $C0653B | m1x0 | 31 | bankc0 |
| EventCmd_EventMemToResult | $C0654E | m1x0 | 26 | bankc0 |
| EventCmd_IfResultEq | $C06568 | m1x0 | 39 | bankc0 |
| EventCmd_GetPc1Id | $C0658F | m1x0 | 27 | bankc0 |
| EventCmd_GetObjectCoords | $C065AA | m1x0 | 65 | bankc0 |
| EventCmd_GetPcCoords | $C065EB | m1x0 | 70 | bankc0 |
| EventCmd_GetObjectFacing | $C065F8 | m1x0 | 39 | bankc0 |
| EventCmd_GetPcFacing | $C0661F | m1x0 | 44 | bankc0 |
| EventCmd_IfObjectDrawn | $C0662C | m1x0 | 41 | bankc0 |
| EventCmd_IfObjectOnScreen | $C06655 | m1x0 | 80 | bankc0 |
| EventCmd_IfAnyButton | $C066A5 | m1x0 | 38 | bankc0 |
| EventCmd_IfPressed02 | $C066B2 | m1x0 | 35 | bankc0 |
| EventCmd_IfPressed80 | $C066BC | m1x0 | 35 | bankc0 |
| EventCmd_IfHeld80 | $C066C6 | m1x0 | 35 | bankc0 |
| EventCmd_IfHeld08 | $C066E5 | m1x0 | 35 | bankc0 |
| EventCmd_IfHeld40 | $C066EF | m1x0 | 35 | bankc0 |
| EventCmd_IfHeld04 | $C066F9 | m1x0 | 33 | bankc0 |
| EventCmd_IfStatusF2_Bit20 | $C06705 | m1x0 | 35 | bankc0 |
| EventCmd_IfStatusF2_Bit10 | $C0670F | m1x0 | 35 | bankc0 |
| EventCmd_IfLatch50_02 | $C06719 | m1x0 | 36 | bankc0 |
| EventCmd_IfLatch50_80 | $C06724 | m1x0 | 39 | bankc0 |
| EventCmd_IfLatch51_80 | $C06732 | m1x0 | 39 | bankc0 |
| EventCmd_IfLatch51_08 | $C06740 | m1x0 | 39 | bankc0 |
| EventCmd_IfLatch51_40 | $C0674E | m1x0 | 39 | bankc0 |
| EventCmd_IfLatch51_04 | $C0675C | m1x0 | 39 | bankc0 |
| EventCmd_IfLatch51_20 | $C0676A | m1x0 | 39 | bankc0 |
| EventCmd_IfLatch51_10 | $C06778 | m1x0 | 37 | bankc0 |
| EventCmd_Op47 | $C06788 | m1x0 | 14 | bankc0 |
| EventCmd_ReadAbs8 | $C06792 | m1x0 | 16 | bankc0 |
| EventScript_ReadAbsSrcPtr | $C067A2 | m1x0 | 37 | bankc0 |
| EventCmd_ReadAbs16 | $C067C7 | m1x0 | 16 | bankc0 |
| EventCmd_WriteAbsImm8 | $C067D7 | m1x0 | 12 | bankc0 |
| EventScript_ReadAbsDestPtr | $C067E3 | m1x0 | 24 | bankc0 |
| EventCmd_WriteAbsImm16 | $C067FB | m1x0 | 17 | bankc0 |
| EventCmd_WriteAbsFromMem8 | $C0680C | m1x0 | 29 | bankc0 |
| EventCmd_WriteAbsFromMem16 | $C06829 | m1x0 | 29 | bankc0 |
| EventCmd_MemCopy | $C06846 | m1x0 | 95 | bankc0 |
| EventCmd_SetMem8 | $C068A5 | m1x0 | 33 | bankc0 |
| EventCmd_SetMem16 | $C068C6 | m1x0 | 35 | bankc0 |
| EventCmd_CopyMem8 | $C068E9 | m1x0 | 42 | bankc0 |
| EventCmd_CopyMem16 | $C06913 | m1x0 | 42 | bankc0 |
| EventCmd_GetEventMem8 | $C0693D | m1x0 | 39 | bankc0 |
| EventCmd_GetEventMem16 | $C06964 | m1x0 | 39 | bankc0 |
| EventCmd_GetStoryline | $C0698B | m1x0 | 27 | bankc0 |
| EventCmd_SetEventMem | $C069A6 | m1x0 | 30 | bankc0 |
| EventCmd_MemToEventMem8 | $C069C4 | m1x0 | 39 | bankc0 |
| EventCmd_MemToEventMem16 | $C069EB | m1x0 | 39 | bankc0 |
| EventCmd_SetStoryline | $C06A12 | m1x0 | 13 | bankc0 |
| EventCmd_AddImm8 | $C06A1F | m1x0 | 38 | bankc0 |
| EventCmd_AddMem8 | $C06A45 | m1x0 | 47 | bankc0 |
| EventCmd_AddMem16 | $C06A74 | m1x0 | 47 | bankc0 |
| EventCmd_SubImm8 | $C06AA3 | m1x0 | 38 | bankc0 |
| EventCmd_SubImm16 | $C06AC9 | m1x0 | 40 | bankc0 |
| EventCmd_SubMem8 | $C06AF1 | m1x0 | 47 | bankc0 |
| EventCmd_SetMemBit | $C06B20 | m1x0 | 43 | bankc0 |
| EventCmd_ClearMemBit | $C06B4B | m1x0 | 43 | bankc0 |
| EventCmd_SetEventBit | $C06B76 | m1x0 | 46 | bankc0 |
| EventCmd_ClearEventBit | $C06BA4 | m1x0 | 46 | bankc0 |
| EventCmd_AndMemImm | $C06BD2 | m1x0 | 37 | bankc0 |
| EventCmd_OrMemImm | $C06BF7 | m1x0 | 37 | bankc0 |
| EventCmd_XorMemImm | $C06C1C | m1x0 | 37 | bankc0 |
| EventCmd_ShiftMemRight | $C06C41 | m1x0 | 40 | bankc0 |
| EventCmd_IncMem8 | $C06C69 | m1x0 | 28 | bankc0 |
| EventCmd_IncMem16 | $C06C85 | m1x0 | 28 | bankc0 |
| EventCmd_DecMem8 | $C06CA1 | m1x0 | 28 | bankc0 |
| EventCmd_SetMemTrue8 | $C06CBD | m1x0 | 25 | bankc0 |
| EventCmd_SetMemTrue16 | $C06CD6 | m1x0 | 26 | bankc0 |
| EventCmd_SetMemFalse | $C06CF0 | m1x0 | 25 | bankc0 |
| EventCmd_GetRandom | $C06D09 | m1x0 | 38 | bankc0 |
| SubroutineCalledByEventCommand | $C06E27 | m1x0 | 53 | bankc0 |
| Obj_RecordMovement | $C09E29 | m1x1 | 91 | bankc0 |
| Player_SelectMoveAnim | $C09E84 | m1x1 | 100 | bankc0 |
| PointersToUn_C09FF2 | $C09FF2 | m1x1 | 135 | bankc0 |
| PointersToUn_C0A079 | $C0A079 | m1x1 | 37 | bankc0 |
| PointersToUn_C0A083 | $C0A083 | m1x1 | 37 | bankc0 |
| PointersToUn_C0A08D | $C0A08D | m1x1 | 37 | bankc0 |
| PointersToUn_C0A097 | $C0A097 | m1x1 | 37 | bankc0 |
| PointersToUn_C0A0A1 | $C0A0A1 | m1x1 | 80 | bankc0 |
| PointersToUn_C0A0AB | $C0A0AB | m1x1 | 80 | bankc0 |
| PointersToUn_C0A0B5 | $C0A0B5 | m1x1 | 80 | bankc0 |
| PointersToUn_C0A0BF | $C0A0BF | m1x1 | 80 | bankc0 |
| PointersToUn_C0A0C9 | $C0A0C9 | m1x1 | 48 | bankc0 |
| PointersToUn_C0A0DE | $C0A0DE | m1x1 | 48 | bankc0 |
| PointersToUn_C0A0F3 | $C0A0F3 | m1x1 | 48 | bankc0 |
| PointersToUn_C0A108 | $C0A108 | m1x1 | 48 | bankc0 |
| PointersToUn_C0A11D | $C0A11D | m1x1 | 91 | bankc0 |
| PointersToUn_C0A132 | $C0A132 | m1x1 | 91 | bankc0 |
| PointersToUn_C0A147 | $C0A147 | m1x1 | 91 | bankc0 |
| PointersToUn_C0A15C | $C0A15C | m1x1 | 91 | bankc0 |
| Field_MoveFollower1 | $C0A26B | m1x1 | 467 | bankc0 |
| Field_MoveFollower2 | $C0A2CE | m1x1 | 477 | bankc0 |
| Obj_ComputeMoveDeltas | $C0ABA2 | m1x0 | 199 | bankc0 |
| Obj_SetVelocityFromAngle | $C0AC69 | m1x0 | 148 | bankc0 |
| Obj_ComputeVelocityFromAngle | $C0ACFD | m1x0 | 563 | bankc0 |
| Field_ActivateVisibleObjects | $C0B0E6 | m1x0 | 217 | bankc0 |
| Battle_ServiceCallEntry | $C18003 | m1x0 | 42 | bankc1 |
| Loc_C1CFE9 | $C1CFE9 | m1x0 | 28 | bankc1 |
| Battle_Inv_AddItem | $C1D005 | m1x0 | 81 | bankc1 |
| Loc_C1D056 | $C1D056 | m1x0 | 48 | bankc1 |
| Loc_C1D086 | $C1D086 | m1x0 | 28 | bankc1 |
| Battle_Gold_Add | $C1D0A2 | m1x0 | 75 | bankc1 |
| Loc_C1D0ED | $C1D0ED | m1x0 | 57 | bankc1 |
| Menu_ShellEntryVector | $C20000 | m1x0 | 55 | bankc2 |
| Menu_InitPpuRegs | $C20043 | m1x0 | 205 | bankc2 |
| Menu_InstallInterruptVectors | $C20110 | m1x0 | 31 | bankc2 |
| Menu_InitShellDpVars | $C2012F | m1x0 | 47 | bankc2 |
| Menu_ResetVramQueue | $C203EF | m1x0 | 22 | bankc2 |
| Menu_WaitFrames | $C20454 | m1x0 | 26 | bankc2 |
| Menu_WaitVblankThunk | $C2046E | m1x0 | 3 | bankc2 |
| Menu_ClearWinSlots | $C20471 | m1x0, m0x0 | 25 | bankc2 |
| Menu_FindFreeSlot | $C2048A | m1x0, m0x0 | 67 | bankc2 |
| MenuTask_SpawnScript | $C204D9 | m1x0 | 34 | bankc2 |
| Overworld_RedrawLayerFull | $C209C5 | m1x0 | 167 | bankc2 |
| Menu_BuildSpriteNodePtrTable | $C20BF6 | m1x0 | 87 | bankc2 |
| Menu_ClearCgramShadow | $C21DB5 | m1x0 | 31 | bankc2 |
| Menu_ResetRngIndex | $C2232D | m1x0 | 9 | bankc2 |
| PointersTo7E_C223EF | $C223EF | m1x0 | 31 | bankc2 |
| PointersTo7E_C22402 | $C22402 | m1x0 | 275 | bankc2 |
| PointersTo7E_C224D5 | $C224D5 | m1x0 | 52 | bankc2 |
| Sub_C2250D | $C2250D | m1x0 | 17 | bankc2 |
| PointersTo7E_C2251E | $C2251E | m1x0 | 56 | bankc2 |
| PointersTo7E_C2258D | $C2258D | m1x0 | 156 | bankc2 |
| PointersTo7E_C2261D | $C2261D | m1x0 | 151 | bankc2 |
| Menu_ClearVram | $C226A8 | m1x0 | 44 | bankc2 |
| Overworld_CacheEventFlags | $C226D6 | m1x0 | 26 | bankc2 |
| Overworld_WritebackEventFlags | $C226F0 | m1x0 | 19 | bankc2 |
| Overworld_SetPartyPixelPos | $C22703 | m1x0 | 29 | bankc2 |
| Overworld_InitCameraScroll | $C22720 | m1x0 | 45 | bankc2 |
| Overworld_SetMapDescPtr | $C2274D | m1x0 | 27 | bankc2 |
| Overworld_LoadMainTileGfx | $C22768 | m1x0 | 84 | bankc2 |
| Overworld_LoadAuxTileGfx | $C227DE | m1x0 | 72 | bankc2 |
| Overworld_LoadGfxSlot08 | $C22826 | m1x0 | 46 | bankc2 |
| Overworld_LoadGfxSlot14 | $C22854 | m1x0 | 46 | bankc2 |
| Overworld_LoadPalettes | $C22882 | m1x0 | 128 | bankc2 |
| Overworld_CopyMemberPalRow | $C228ED | m0x0 | 21 | bankc2 |
| Overworld_LoadGfxSlot07 | $C22902 | m1x0 | 46 | bankc2 |
| Overworld_LoadGfxSlot0B | $C22930 | m1x0 | 46 | bankc2 |
| Overworld_LoadMetatileDefs | $C2295E | m1x0 | 46 | bankc2 |
| Overworld_LoadTilemaps | $C2298C | m1x0 | 46 | bankc2 |
| Overworld_LoadTileProps | $C229BA | m1x0 | 46 | bankc2 |
| Overworld_LoadRegionMap | $C229E8 | m1x0 | 46 | bankc2 |
| Overworld_LoadTriggerScripts | $C22A16 | m1x0 | 46 | bankc2 |
| Overworld_LoadTriggerTables | $C22A44 | m1x0 | 285 | bankc2 |
| Overworld_LoadPartySpriteGfx | $C22B61 | m1x0 | 91 | bankc2 |
| Overworld_CopyMemberTilePage | $C22BBC | m0x0 | 19 | bankc2 |
| Overworld_CopyMemberSpritePal | $C22BCF | m0x0 | 44 | bankc2 |
| Menu_ClearWram8621Block | $C22BFB | m1x0 | 34 | bankc2 |
| MenuEngine_SetIdle | $C22C1D | m1x0 | 118 | bankc2 |
| Overworld_ReloadMapGfx | $C22C93 | m1x0 | 46 | bankc2 |
| Overworld_LoadAllMapGfx | $C22CC1 | m1x0 | 209 | bankc2 |
| Overworld_DmaBufToVram | $C22D70 | m0x0 | 33 | bankc2 |
| Overworld_ApplyStorylineGfx | $C22D91 | m1x0 | 183 | bankc2 |
| Overworld_SaveWramState | $C22E23 | m1x0 | 79 | bankc2 |
| Overworld_RestoreWramState | $C22E72 | m1x0 | 79 | bankc2 |
| Text_Init8600ListHeader | $C25775 | m1x0 | 35 | bankc2 |
| Text_Clear8621Words | $C25798 | m1x0 | 24 | bankc2 |
| Text_RenderString | $C257DF | m1x0 | 68 | bankc2 |
| Text_EngineTick | $C25823 | m1x0 | 31 | bankc2 |
| Loc_C2631F | $C2631F | m1x0 | 192 | bankc2 |
| Loc_C263DF | $C263DF | m1x0 | 33 | bankc2 |
| Loc_C26400 | $C26400 | m0x0 | 83 | bankc2 |
| Loc_C2645C | $C2645C | m1x0 | 226 | bankc2 |
| Loc_C265B2 | $C265B2 | m0x0 | 186 | bankc2 |
| Sub_C268F4 | $C268F4 | m1x0 | 113 | bankc2 |
| Sub_C26965 | $C26965 | m1x0 | 139 | bankc2 |
| Sub_C26A34 | $C26A34 | m1x0 | 148 | bankc2 |
| Sub_C271F9 | $C271F9 | m1x0 | 39 | bankc2 |
| Sub_C2722C | $C2722C | m1x0 | 40 | bankc2 |
| Sub_C27254 | $C27254 | m1x0 | 100 | bankc2 |
| Sub_C272D8 | $C272D8 | m1x0 | 73 | bankc2 |
| Sub_C27361 | $C27361 | m1x0 | 40 | bankc2 |
| Sub_C27389 | $C27389 | m1x0 | 209 | bankc2 |
| Loc_C27B5A | $C27B5A | m1x0 | 37 | bankc2 |
| Loc_C27B7F | $C27B7F | m1x0 | 32 | bankc2 |
| Party_DispatchVector | $C28004 | m1x0, m1x1 | 6 | bankc2 |
| Menu_LoadPartyD1Recs | $C2834D | m0x0, m1x1, m1x0 | 56 | bankc2 |
| Item_AddToInventory | $C28791 | m1x0 | 68 | bankc2 |
| Item_RemoveFromInventory | $C287D5 | m1x0 | 45 | bankc2 |
| Item_FindInventorySlot | $C287FA | m1x1 | 28 | bankc2 |
| Item_ClassifyId | $C28881 | m0x0, m1x0 | 38 | bankc2 |
| Party_DispatchOp | $C28C36 | m1x0, m1x1 | 37 | bankc2 |
| Sub_C28C5A | $C28C5A | m1x1 | 1 | bankc2 |
| Party_CheckActiveCharacter | $C28C75 | m1x1 | 24 | bankc2 |
| Party_AddActiveCharacter | $C28C79 | m1x1 | 58 | bankc2 |
| Party_RemoveCharacter | $C28CA2 | m1x1 | 45 | bankc2 |
| Party_CheckRosterCharacter | $C28CCF | m1x1 | 22 | bankc2 |
| Sub_C28CE5 | $C28CE5 | m1x1 | 33 | bankc2 |
| Party_MarkRosterInactive | $C28CF7 | m1x1 | 14 | bankc2 |
| Party_RecalcAllHpFromEquipment | $C28D05 | m1x1 | 23 | bankc2 |
| Party_RecalcCharHpBonus | $C28D1C | m0x0 | 41 | bankc2 |
| Party_RestoreAllMpToBaseMax | $C28D45 | m1x1 | 30 | bankc2 |
| Sub_C28D63 | $C28D63 | m1x1 | 7 | bankc2 |
| Sub_C28D6A | $C28D6A | m1x1 | 37 | bankc2 |
| Sub_C28D8F | $C28D8F | m1x1 | 139 | bankc2 |
| Sub_C28E1E | $C28E1E | m1x1 | 15 | bankc2 |
| Mode7Engine_Entry | $C30000 | m1x0 | 101 | bankc3 |
| Gfx_DecompressEntry | $C30557 | m1x0, m0x0 | 860 | bankc3 |
| Audio_Init_Entry | $C70000 | m1x0 | 267 | bankc7 |
| MainInit | $FDC000 | m1x0 | 215 | bankfd |
| FdCore_EffectPpuSetup | $FDC0D7 | m1x0 | 77 | bankfd |
| FdCore_EffectStateInit | $FDC124 | m1x0 | 202 | bankfd |
| FdCore_SetupHdmaChannels | $FDC1EE | m1x0 | 211 | bankfd |
| FdCore_FrameTick | $FDC2C1 | m1x1 | 30 | bankfd |
| FdCore_BuildHdmaTablesMode0A | $FDC2EB | m1x1 | 334 | bankfd |
| FdCore_BuildWin2HdmaA | $FDC439 | m1x0 | 62 | bankfd |
| FdCore_BuildWin2HdmaB | $FDC477 | m1x0 | 62 | bankfd |
| FdCore_BuildBg2ScrollHdmaA | $FDC4B5 | m1x0 | 121 | bankfd |
| FdCore_BuildBg2ScrollHdmaB | $FDC52E | m1x0 | 121 | bankfd |
| FdCore_UpdateScrollHdmaDataA | $FDC5A7 | m1x0 | 336 | bankfd |
| FdCore_UpdateScrollHdmaDataB | $FDC6F7 | m1x0 | 336 | bankfd |
| FdCore_BuildHdmaTablesMode0B | $FDC847 | m1x1 | 334 | bankfd |
| Sub_FDC995 | $FDC995 | m1x1 | 1041 | bankfd |
| FdCore_WaveLineIndexReset | $FDCC58 | m1x0 | 6 | bankfd |
| FdCore_AppendBg3WaveEntriesA | $FDCC5E | m1x0 | 84 | bankfd |
| FdCore_WaveLineIndexAdvance | $FDCCB2 | m1x0 | 6 | bankfd |
| FdCore_AppendBg3WaveEntriesB | $FDCCB8 | m1x0 | 84 | bankfd |
| Sub_FDCD0C | $FDCD0C | m1x1 | 1041 | bankfd |
| Sub_FDCFCF | $FDCFCF | m1x1 | 1021 | bankfd |
| Sub_FDD27E | $FDD27E | m1x1 | 1021 | bankfd |
| FdCore_BuildRowSplitTable | $FDD52D | m1x0 | 167 | bankfd |
| Warp_SceneInit | $FDDB97 | m1x0 | 294 | bankfd |
| Warp_BackupWram7F | $FDEAA6 | m1x0 | 17 | bankfd |
| Boot_InstallVectors | $FDEAF4 | m1x0 | 31 | bankfd |
| FdCore_PpuInit | $FDEB13 | m1x0 | 120 | bankfd |
| Warp_ClearSpiralBuffer | $FDECC0 | m1x0 | 21 | bankfd |
| Warp_BuildGatePalette | $FDEF6F | m1x0 | 146 | bankfd |
| Warp_DmaTilemapLow | $FDF1C1 | m1x0 | 44 | bankfd |
| Warp_GenTunnelTexture | $FDF1ED | m1x0 | 23 | bankfd |
| Warp_GenTextureRows | $FDF204 | m1x0 | 30 | bankfd |
| Warp_InitShadowOam | $FDF24C | m1x0 | 26 | bankfd |
| Sub_FDFFE5 | $FDFFE5 | m1x0 | 3 | bankfd |
| Warp_TransitionShortStub | $FDFFE8 | m1x0 | 3 | bankfd |
| Warp_GateEntryStub | $FDFFEB | m1x0 | 3 | bankfd |
| Warp_TransitionStub | $FDFFF1 | m1x0 | 3 | bankfd |
| PalAnim_LoadMapConfigStub | $FDFFF4 | m1x0 | 3 | bankfd |
| AnimTile_FrameTickStub | $FDFFF7 | m1x0 | 3 | bankfd |
| FdCore_ResourceServiceStub | $FDFFFA | m1x0 | 3 | bankfd |
| Field_FrameUpdate | $C000BF | m1x0 | 183 | bankc0 |
| Field_ReadHVLatch | $C05A46 | m1x1 | 77 | bankc0 |
| EventScript_RunObjectScriptSlice | $C05A93 | m1x1 | 50 | bankc0 |
| Field_UploadCgram | $C0712E | m1x0 | 38 | bankc0 |
| Vblank_UploadRightColL1 | $C086AB | m1x0 | 50 | bankc0 |
| Dma7_TriggerVramWrite_Inc32 | $C086DD | m1x0 | 26 | bankc0 |
| Vblank_UploadRightColL2 | $C086F7 | m1x0 | 50 | bankc0 |
| Vblank_UploadRightColL3 | $C08729 | m1x0 | 50 | bankc0 |
| Player_ProcessControl | $C0881E | m1x0, m0x0 | 60 | bankc0 |
| Player_ApplyPadMoveIntent | $C088EE | m1x0 | 20 | bankc0 |
| Sub_C08924 | $C08924 | m1x1 | 1 | bankc0 |
| Sub_C08925 | $C08925 | m1x1 | 31 | bankc0 |
| Sub_C08944 | $C08944 | m1x1 | 31 | bankc0 |
| Sub_C08963 | $C08963 | m1x1 | 31 | bankc0 |
| Sub_C08982 | $C08982 | m1x1 | 31 | bankc0 |
| Sub_C089A1 | $C089A1 | m1x1 | 49 | bankc0 |
| Sub_C089D2 | $C089D2 | m1x1 | 53 | bankc0 |
| Sub_C08A07 | $C08A07 | m1x1 | 53 | bankc0 |
| Sub_C08A3C | $C08A3C | m1x1 | 49 | bankc0 |
| Field_UpdateDrawObjects | $C0A810 | m1x1 | 125 | bankc0 |
| Vblank_UploadOam | $C0ECA3 | m1x0 | 41 | bankc0 |
| Vblank_UpdateAnimTileWords | $C0ED15 | m1x0 | 841 | bankc0 |
| Dialog_DrawChoiceCursor | $C0F05E | m1x0 | 178 | bankc0 |
| Dialog_DrawChoiceSlot3 | $C0F110 | m1x0 | 27 | bankc0 |
| Dialog_DrawChoiceSlot2 | $C0F12B | m1x0 | 29 | bankc0 |
| Dialog_DrawChoiceSlot1 | $C0F142 | m1x0 | 29 | bankc0 |
| Dialog_DrawChoiceSlot0 | $C0F159 | m1x0 | 29 | bankc0 |
| Menu_MarkEquippedRocks | $C282E1 | m1x0, m0x0 | 63 | bankc2 |
| Pad_ApplyButtonRemap | $C28545 | m0x1 | 105 | bankc2 |
| Pad_RemapOneSet | $C28555 | m1x1 | 89 | bankc2 |
| Menu_TickPlayTimeClock | $C285AE | m1x1, m0x1 | 34 | bankc2 |
| NewGame_SeedCharStatsAndTechs | $C2956E | m1x1, m1x0 | 77 | bankc2 |
| NewGame_InitFlagsAndSettings | $C295BB | m0x0, m1x1 | 80 | bankc2 |
| Mode7_PlayPendingSong | $C309A4 | m1x0 | 53 | bankc3 |
| Mode7_List0920_Remove | $C30CB8 | m0x0 | 21 | bankc3 |
| Mode7_List0920_Add | $C30CE2 | m0x0 | 22 | bankc3 |
| Mode7_List0940_Add | $C30CF8 | m0x0 | 22 | bankc3 |
| Sub_C30D0E | $C30D0E | m0x0 | 80 | bankc3 |
| Sub_C30D5E | $C30D5E | m0x0 | 94 | bankc3 |
| Audio_UploadInstrumentSample | $C70655 | m0x0 | 200 | bankc7 |
| Audio_NormalizeSongCmd | $C70734 | m1x0 | 33 | bankc7 |
| Apu_SendByteWaitAck | $C709DA | m1x0 | 16 | bankc7 |
| Audio_SetEchoLimit | $C709FD | m1x0 | 21 | bankc7 |
| Audio_EvictSamplesAboveLimit | $C70A12 | m1x0 | 39 | bankc7 |
| PalAnim_LoadScriptedFrame | $FDE485 | m1x0 | 99 | bankfd |
| PalAnim_ScaleBrightnessStep | $FDE5A4 | m1x0 | 171 | bankfd |
| PalAnim_ScaleRGB | $FDE64F | m1x0 | 82 | bankfd |
| Loc_FDE6A1 | $FDE6A1 | m1x0 | 48 | bankfd |
| Loc_FDE6D1 | $FDE6D1 | m1x0 | 55 | bankfd |
| PalAnim_ScaleRed | $FDE708 | m1x0 | 50 | bankfd |
| Loc_FDE73A | $FDE73A | m1x0 | 63 | bankfd |
| PalAnim_ScaleRedGreen | $FDE779 | m1x0 | 71 | bankfd |
| Loc_FDE7C0 | $FDE7C0 | m1x0 | 71 | bankfd |
| PalAnim_TintToWhiteStep | $FDE82C | m1x0 | 171 | bankfd |
| PalAnim_WhitenRGB | $FDE8D7 | m1x0 | 94 | bankfd |
| PalAnim_WhitenBlue | $FDE935 | m1x0 | 45 | bankfd |
| PalAnim_WhitenRed | $FDE962 | m1x0 | 45 | bankfd |
| Loc_FDE98F | $FDE98F | m1x0 | 59 | bankfd |
| Loc_FDE9CA | $FDE9CA | m1x0 | 71 | bankfd |
| PalAnim_WhitenGreenBlue | $FDEA11 | m1x0 | 75 | bankfd |
| PalAnim_WhitenRedGreen | $FDEA5C | m1x0 | 74 | bankfd |
| Sub_FDFFEE | $FDFFEE | m1x0 | 32 | bankfd |
| FdCore_VramDmaServiceStub | $FDFFFD | m1x0 | 3 | bankfd |
| Menu_CheckRockTriples | $FFF958 | m1x0, m0x0, m1x1 | 99 | bankff |
| SaveSlot_Validate | $FFF9C4 | m1x1, m1x0 | 55 | bankff |
| SaveSlot_GetBase | $FFFBD3 | m0x0 | 14 | bankff |
| SaveSlot_Checksum | $FFFBE7 | m0x0 | 29 | bankff |
| Sub_00FF10 | $00FF10 | m0x0, m1x0, m1x1 | 4 | bank00 |
| Sub_00FF14 | $00FF14 | m1x0, m0x0, m1x1 | 4 | bank00 |
| Field_EntryVec02_UpdateBgAutoScroll | $C00002 | m1x0 | 394 | bankc0 |
| Field_EntryVec08_StartLocationSong | $C00008 | m1x0 | 62 | bankc0 |
| Sub_01A5 | $C001A5 | m1x0 | 167 | bankc0 |
| Field_PrepareBattleEntry | $C0024C | m1x0 | 55 | bankc0 |
| Loc_C0034B | $C0034B | m1x0 | 100 | bankc0 |
| Sub_0C76 | $C00C76 | m1x0 | 3353 | bankc0 |
| Player_TryInteract | $C01AAC | m1x0 | 231 | bankc0 |
| Player_FindFacingObject | $C01CFC | m1x0 | 90 | bankc0 |
| Sub_C01D84 | $C01D84 | m1x1 | 39 | bankc0 |
| Sub_C01DAB | $C01DAB | m1x1 | 42 | bankc0 |
| Sub_C01EC7 | $C01EC7 | m0x0 | 25 | bankc0 |
| Sub_C01EE0 | $C01EE0 | m0x0 | 25 | bankc0 |
| Field_TickScreenEffect | $C021E1 | m1x0 | 13 | bankc0 |
| Dialog_UploadTextTileRow | $C06ECB | m1x0 | 38 | bankc0 |
| Obj_AssignSpriteTileSlot | $C06F9A | m1x1, m0x1 | 13 | bankc0 |
| Sub_C06FB9 | $C06FB9 | m1x1, m0x1 | 1 | bankc0 |
| Sub_C06FBA | $C06FBA | m1x1, m0x1 | 156/10 | bankc0 |
| Obj_AssignSpritePalette | $C07170 | m1x1 | 17 | bankc0 |
| Sub_C07199 | $C07199 | m1x1 | 121 | bankc0 |
| Sub_C0721C | $C0721C | m1x1 | 152 | bankc0 |
| Vblank_UploadBottomRowL1 | $C08445 | m1x0 | 98 | bankc0 |
| Dma7_TriggerVramWrite_Inc1 | $C084A7 | m1x0 | 26 | bankc0 |
| Vblank_UploadBottomRowL2 | $C084C1 | m1x0 | 98 | bankc0 |
| Vblank_UploadBottomRowL3 | $C08523 | m1x0 | 98 | bankc0 |
| Vblank_UploadTopRowL1 | $C08585 | m1x0 | 98 | bankc0 |
| Vblank_UploadTopRowL2 | $C085E7 | m1x0 | 98 | bankc0 |
| Vblank_UploadTopRowL3 | $C08649 | m1x0 | 98 | bankc0 |
| Vblank_UploadLeftColL1 | $C0875B | m1x0 | 50 | bankc0 |
| Vblank_UploadLeftColL2 | $C0878D | m1x0 | 50 | bankc0 |
| Vblank_UploadLeftColL3 | $C087BF | m1x0 | 50 | bankc0 |
| Obj_UpdateVisibility | $C0A88D | m1x1 | 181 | bankc0 |
| Obj_ActivateIfOnScreen | $C0B127 | m1x1 | 107 | bankc0 |
| Battle_Main | $C10000 | m1x0 | 7 | bankc1 |
| BattleSys_ServiceEntry | $C10003 | m1x0 | 3 | bankc1 |
| BattleSys_EntryVec09 | $C10009 | m1x0 | 7 | bankc1 |
| BattleSys_FrameTickEntry | $C1000C | m1x0 | 7 | bankc1 |
| BattleSys_EntryVec0F | $C1000F | m1x0 | 7 | bankc1 |
| BattleSys_EntryVec18 | $C10018 | m1x0 | 9 | bankc1 |
| Sub_C10023 | $C10023 | m1x0 | 275 | bankc1 |
| BattleSys_ServiceDispatch | $C10045 | m1x0 | 12 | bankc1 |
| BattleSys_PumpFrames | $C1007E | m1x0 | 11 | bankc1 |
| Sub_C1106E | $C1106E | m1x0 | 117 | bankc1 |
| Sub_C11B19 | $C11B19 | m1x0 | 60 | bankc1 |
| BattleUI_RenderGaugeUnlessMode3 | $C11C4A | m1x0 | 815 | bankc1 |
| BattleUI_RenderPanelStrip | $C11C55 | m1x0 | 804 | bankc1 |
| Battle_CacheBattlerCoordsAll | $C1283D | m1x0 | 29 | bankc1 |
| Battle_GaugeRemaining | $C1285A | m1x0 | 86 | bankc1 |
| EnemyAI_SpatialService | $C12986 | m1x0 | 44 | bankc1 |
| AISpatial02_NearestEnemy | $C12A43 | m1x0 | 85 | bankc1 |
| AISpatial_DistSqCheck0400 | $C12B37 | m1x0 | 133 | bankc1 |
| Battle_TickPcSlots | $C12D9F | m1x0 | 1011 | bankc1 |
| Battle_UpdatePcFacingToNearest | $C12F22 | m1x0 | 117 | bankc1 |
| Battle_EvalPcSlotIncapacity | $C12F97 | m1x0 | 197 | bankc1 |
| Battle_ApplyBattlerActionState | $C1308C | m1x0 | 42 | bankc1 |
| BattleUI_InitPanelStrips | $C134A7 | m1x0 | 52 | bankc1 |
| Battle_FacePcsToNearest | $C1350F | m1x0 | 62 | bankc1 |
| BattleSys_HalfRateSlotTick | $C1369A | m1x0 | 122 | bankc1 |
| Battle_TickStatusEffectVisuals | $C13714 | m1x0 | 39 | bankc1 |
| Battle_DispatchStatusVisual | $C1373B | m1x0 | 37 | bankc1 |
| Loc_C18461 | $C18461 | m1x0 | 988 | bankc1 |
| Battle_ProcessAF0AQueue | $C1883D | m1x0 | 37 | bankc1 |
| Battle_ClearB192Window | $C18862 | m1x0 | 20 | bankc1 |
| Sub_C18A05 | $C18A05 | m1x0 | 76 | bankc1 |
| Battle_ScanSlotsForNextActions | $C18C09 | m1x0 | 53 | bankc1 |
| EnemyAI_CondMet | $C18C3E | m1x0 | 174 | bankc1 |
| Battle_BeginPcActionExec | $C18CF9 | m1x0 | 430 | bankc1 |
| PointersToEn_C18EA7 | $C18EA7 | m1x0 | 4 | bankc1 |
| AIPrep01_Attack2 | $C1983A | m1x0 | 138 | bankc1 |
| AITgt05_RandomPC | $C1A452 | m1x0 | 93 | bankc1 |
| EnemyAI_BuildTargetList | $C1AC14 | m1x0 | 50 | bankc1 |
| EnemyAI_ClearTargetList | $C1AC46 | m1x0 | 17 | bankc1 |
| Battle_SysCmd4CommitResults | $C1AC57 | m1x0 | 7 | bankc1 |
| Battle_MarkPendingEnemyReactions | $C1AC5E | m1x0 | 39 | bankc1 |
| Battle_ApplyResultsWrapper | $C1AC85 | m1x0 | 4 | bankc1 |
| EnemyAI_TargetMaskPrimary | $C1AD09 | m1x0 | 44 | bankc1 |
| EnemyAI_TargetMaskSecondary | $C1AD35 | m1x0 | 51 | bankc1 |
| Battle_RandRange | $C1AF22 | m1x0 | 87 | bankc1 |
| EnemyAI_RunActiveSection | $C1AFD2 | m1x0 | 193 | bankc1 |
| Battle_UpdateNearFatalFlag | $C1B093 | m1x0 | 35 | bankc1 |
| Battle_SyncStatusTickFlags | $C1B0B6 | m1x0 | 365 | bankc1 |
| Battle_ProcessDeathsAllSlots | $C1B223 | m1x0 | 86 | bankc1 |
| Battle_ProcessSlotDeath | $C1B279 | m1x0 | 322 | bankc1 |
| Battle_CancelPcCommandIfDisabled | $C1B3F9 | m1x0 | 73 | bankc1 |
| Battle_SetAiPtrStatusOverride | $C1B4AA | m1x0 | 63 | bankc1 |
| Battle_BuildComboPartnerLists | $C1B575 | m1x0 | 348 | bankc1 |
| Battle_RefundPendingItem | $C1B6D1 | m1x0 | 60 | bankc1 |
| Battle_InitPcActionFlags | $C1B70E | m1x0 | 23 | bankc1 |
| Battle_InitPcMenuBlocks | $C1B725 | m1x0 | 61 | bankc1 |
| Battle_PumpEnemyDeathReactions | $C1B762 | m1x0 | 144 | bankc1 |
| Battle_SysCmd2Long | $C1B961 | m1x0 | 6 | bankc1 |
| Battle_RunNextReadyPcAction | $C1B967 | m1x0 | 880 | bankc1 |
| Battle_PendingEquipStub | $C1BC60 | m1x0 | 3 | bankc1 |
| Battle_ResetPcAtbAfterAction | $C1BCE1 | m1x0 | 142 | bankc1 |
| Battle_AdjustAtbRateForStatus | $C1BD6F | m1x0 | 68 | bankc1 |
| Battle_CancelPcActionRefundAtb | $C1BDB3 | m1x0 | 403 | bankc1 |
| Battle_DispatchItemEffectClass | $C1BF46 | m1x0 | 51 | bankc1 |
| Battle_PrepTechAction | $C1BF79 | m1x0 | 8 | bankc1 |
| Battle_SysCmd4 | $C1BFA4 | m1x0 | 6 | bankc1 |
| Battle_QueueBasicAttack | $C1BFAA | m1x0 | 128 | bankc1 |
| Battle_BuildActionTargetList | $C1C1DD | m1x0 | 1364 | bankc1 |
| Tech_LoadActorParams | $C1C74C | m1x0 | 65 | bankc1 |
| Battle_ReacquireSingleTarget | $C1C82D | m1x0 | 202 | bankc1 |
| Battle_ExpandTargetMask | $C1C8F7 | m1x0 | 20 | bankc1 |
| Battle_Mul16 | $C1C90B | m1x0 | 31 | bankc1 |
| Battle_Div16 | $C1C92A | m1x1, m1x0 | 50 | bankc1 |
| Tech_BuildSingleTechEntries | $C1C96A | m1x0 | 176 | bankc1 |
| Tech_BuildDualTechEntries | $C1CA1A | m1x0 | 302 | bankc1 |
| Tech_GetMpCostSlot0 | $C1CB48 | m1x0 | 25 | bankc1 |
| Tech_GetMpCostSlot1 | $C1CB61 | m1x0 | 25 | bankc1 |
| Tech_GetMpCostSlot2 | $C1CB7A | m1x0 | 25 | bankc1 |
| Tech_GetTargetData | $C1CB93 | m1x0 | 48 | bankc1 |
| Tech_ApplyMpDiscount | $C1CBF6 | m1x0 | 21 | bankc1 |
| Loc_C1CC3E | $C1CC3E | m1x0 | 51 | bankc1 |
| Tech_DeductMpFromParticipant | $C1CC71 | m1x0 | 90 | bankc1 |
| Tech_BuildTripleTechEntries | $C1CCCB | m1x0 | 308 | bankc1 |
| Battle_BuildItemMenuList | $C1CDFF | m1x0 | 55 | bankc1 |
| Battle_RecalcEquipStats | $C1CE3A | m1x0 | 219 | bankc1 |
| Battle_ReloadPartyCharInitData | $C1CF15 | m1x0 | 60 | bankc1 |
| Battle_ApplyGearStatBonus | $C1CF69 | m1x0 | 85 | bankc1 |
| Battle_ItemEffect_Heal | $C1D490 | m1x0 | 29 | bankc1 |
| Battle_ItemEffect_CureStatus | $C1D4AD | m1x0 | 41 | bankc1 |
| Loc_C1D4D6 | $C1D4D6 | m1x0 | 29 | bankc1 |
| Battle_ExecPlayerTechAction | $C1D523 | m1x0 | 556 | bankc1 |
| Tech_ValidateParticipantsMp | $C1D74F | m1x0 | 117 | bankc1 |
| Battle_IndexTargetEntry0E | $C1E89F | m1x0 | 33 | bankc1 |
| Battle_SetAttackerBase | $C1E9A3 | m1x0 | 21 | bankc1 |
| Battle_NegateDamageIfHeal | $C1EBF8 | m1x0 | 65 | bankc1 |
| Battle_ClearConsumedResults | $C1EC39 | m1x0 | 70 | bankc1 |
| Battle_ApplyPendingResults | $C1EC7F | m1x0 | 266 | bankc1 |
| Battle_InitCombatantRecords | $C1FA8B | m1x0 | 803 | bankc1 |
| Battle_AylaFistWeaponId | $C1FDAE | m1x0 | 17 | bankc1 |
| Math_Multiply_Trampoline | $C1FDBF | m1x0 | 4 | bankc1 |
| Battle_RefundPendingItem_Trampoline | $C1FDCF | m1x0 | 4 | bankc1 |
| Menu_WaitVblank | $C20346 | m1x0 | 7 | bankc2 |
| Menu_HideAllSprites | $C2034D | m0x0 | 162 | bankc2 |
| Menu_NmiFlushVramQueue | $C20405 | m1x0 | 79 | bankc2 |
| Menu_FindFreeSlotPri | $C204CD | m1x0 | 69 | bankc2 |
| MenuTask_SpawnScriptPri | $C204FB | m1x0 | 34 | bankc2 |
| Overworld_ScrollLayerH | $C20568 | m1x0 | 53 | bankc2 |
| Overworld_ScrollLayerHDec | $C2059D | m1x0 | 106 | bankc2 |
| Overworld_ScrollLayerHInc | $C20607 | m1x0 | 101 | bankc2 |
| Overworld_ScrollLayerV | $C2066C | m1x0 | 53 | bankc2 |
| Overworld_ScrollLayerVDec | $C206A1 | m1x0 | 106 | bankc2 |
| Overworld_ScrollLayerVInc | $C2070B | m1x0 | 104 | bankc2 |
| Overworld_BuildColStripLeft | $C20773 | m0x0 | 79 | bankc2 |
| Overworld_BuildColStripRight | $C207C2 | m0x0 | 79 | bankc2 |
| Overworld_SetupColStrip | $C20811 | m0x0 | 50 | bankc2 |
| Overworld_BuildRowStripTop | $C20843 | m0x0 | 76 | bankc2 |
| Overworld_BuildRowStripBottom | $C2088F | m0x0 | 76 | bankc2 |
| Overworld_SetupRowStrip | $C208DB | m0x0 | 50 | bankc2 |
| Overworld_QueueColStripDma | $C2090D | m0x0 | 84 | bankc2 |
| Overworld_QueueRowStripDma | $C20961 | m0x0 | 100 | bankc2 |
| Overworld_BuildColumnPair | $C20A6C | m0x0 | 66 | bankc2 |
| Overworld_EmitMetatileTiles | $C20AAE | m0x0 | 46 | bankc2 |
| Overworld_UploadColumnPair | $C20ADC | m0x0 | 119 | bankc2 |
| Menu_InsertSpriteNode | $C20B53 | m0x0 | 163 | bankc2 |
| Menu_InitSpriteFreeList | $C20C4D | m1x0 | 157 | bankc2 |
| Menu_BuildAllSpriteOam | $C20CEA | m1x0 | 30 | bankc2 |
| Menu_BuildSpriteOam | $C20D08 | m1x0 | 261 | bankc2 |
| Interp_BankC2_Bytecode | $C20E1D | m0x0, m1x0 | 45 | bankc2 |
| Sub_C20E5A | $C20E5A | m0x0 | 17 | bankc2 |
| Sub_C20E6B | $C20E6B | m0x0 | 19 | bankc2 |
| Sub_C20E91 | $C20E91 | m0x0 | 18 | bankc2 |
| Sub_C20EA3 | $C20EA3 | m0x0 | 57 | bankc2 |
| Menu_ApplyWinVelocity | $C20F38 | m0x0 | 43 | bankc2 |
| Sub_C21037 | $C21037 | m0x0 | 90 | bankc2 |
| Sub_C21091 | $C21091 | m0x0 | 23 | bankc2 |
| Sub_C210A8 | $C210A8 | m0x0 | 23 | bankc2 |
| Sub_C210CD | $C210CD | m0x0 | 51 | bankc2 |
| Sub_C21100 | $C21100 | m0x0 | 105 | bankc2 |
| Sub_C211F6 | $C211F6 | m0x0 | 22 | bankc2 |
| Sub_C2120C | $C2120C | m0x0 | 17 | bankc2 |
| Sub_C2121D | $C2121D | m0x0 | 19 | bankc2 |
| Sub_C21243 | $C21243 | m0x0 | 20 | bankc2 |
| Sub_C2126D | $C2126D | m0x0 | 24 | bankc2 |
| Sub_C21285 | $C21285 | m0x0 | 17 | bankc2 |
| Sub_C21296 | $C21296 | m0x0 | 17 | bankc2 |
| Sub_C212B8 | $C212B8 | m0x0 | 22 | bankc2 |
| Sub_C2136A | $C2136A | m0x0 | 11 | bankc2 |
| Sub_C21375 | $C21375 | m0x0 | 35 | bankc2 |
| Sub_C2148B | $C2148B | m0x0 | 34 | bankc2 |
| Sub_C214F7 | $C214F7 | m0x0 | 37 | bankc2 |
| Sub_C2151C | $C2151C | m0x0 | 37 | bankc2 |
| Sub_C21541 | $C21541 | m0x0 | 37 | bankc2 |
| Sub_C21566 | $C21566 | m0x0 | 37 | bankc2 |
| Sub_C2158B | $C2158B | m0x0 | 14 | bankc2 |
| Sub_C21599 | $C21599 | m0x0 | 14 | bankc2 |
| Sub_C215C3 | $C215C3 | m0x0 | 28 | bankc2 |
| Sub_C215E3 | $C215E3 | m0x0 | 22 | bankc2 |
| Sub_C215F9 | $C215F9 | m0x0 | 22 | bankc2 |
| Sub_C2160F | $C2160F | m0x0 | 14 | bankc2 |
| Sub_C2161D | $C2161D | m0x0 | 46 | bankc2 |
| Sub_C2164B | $C2164B | m0x0 | 175 | bankc2 |
| Menu_ResetWinPixelAccum | $C21759 | m0x0 | 43 | bankc2 |
| Menu_GetWinPixelDeltaX | $C21784 | m0x0 | 39 | bankc2 |
| Menu_GetWinPixelDeltaY | $C217AB | m0x0 | 39 | bankc2 |
| Sub_C21839 | $C21839 | m0x0 | 15 | bankc2 |
| Sub_C21848 | $C21848 | m0x0 | 23 | bankc2 |
| Sub_C2185F | $C2185F | m0x0 | 12 | bankc2 |
| Sub_C2186B | $C2186B | m0x0 | 35 | bankc2 |
| Sub_C2188E | $C2188E | m0x0 | 38 | bankc2 |
| Sub_C218C7 | $C218C7 | m0x0 | 35 | bankc2 |
| Sub_C218F1 | $C218F1 | m0x0 | 46 | bankc2 |
| Sub_C21947 | $C21947 | m0x0 | 18 | bankc2 |
| Sub_C21A2D | $C21A2D | m0x0 | 22 | bankc2 |
| Sub_C21C81 | $C21C81 | m0x0 | 3 | bankc2 |
| Overworld_WrapPosToMap | $C21CB8 | m0x0 | 34 | bankc2 |
| Menu_SetWinAnimScript | $C21CDA | m0x0, m1x0 | 27 | bankc2 |
| Menu_GetRandom | $C22336 | m1x0 | 15 | bankc2 |
| Menu_TrySendAudioCmd | $C22ED9 | m1x0 | 54 | bankc2 |
| Overworld_FindTriggersAtPos | $C232CD | m0x0 | 124 | bankc2 |
| Overworld_DispatchTriggers | $C23349 | m0x0 | 53 | bankc2 |
| Overworld_InitPosHistory | $C2391C | m0x0 | 24 | bankc2 |
| Overworld_PushPosHistory | $C23934 | m0x0 | 31 | bankc2 |
| Overworld_SetMemberWalkAnim | $C23953 | m1x0 | 44 | bankc2 |
| Overworld_SetMemberIdleAnim | $C23987 | m1x0, m0x0 | 35 | bankc2 |
| Overworld_CheckCoverTiles | $C239AE | m1x0 | 21 | bankc2 |
| Overworld_CheckMoveBlocked | $C239C3 | m1x0 | 18 | bankc2 |
| Overworld_FetchTileProps | $C239D5 | m0x0 | 129 | bankc2 |
| Overworld_SetStepTarget | $C23A56 | m0x0 | 57 | bankc2 |
| Overworld_WrapStepTarget | $C23A8F | m0x0 | 34 | bankc2 |
| Overworld_StorePartyTilePos | $C23ACB | m0x0 | 23 | bankc2 |
| Overworld_FetchExitParam | $C26263 | m1x0 | 11 | bankc2 |
| HwMath_MulBy60 | $C262AE | m0x0, m1x0 | 63 | bankc2 |
| Overworld_GetRegionId | $C262ED | m0x0, m1x0 | 50 | bankc2 |
| Pad_ServiceLongEntry | $C28002 | m1x0 | 6 | bankc2 |
| MenuOverlay_InitShell | $C28096 | m1x0 | 212 | bankc2 |
| PartyRoster_Rebuild | $C2816A | m1x0 | 38 | bankc2 |
| MenuTask_SetSleepFrames | $C28249 | m0x0 | 12 | bankc2 |
| MenuTask_KillByMask | $C28255 | m1x0 | 23 | bankc2 |
| DmaQueue_AppendFromRom | $C28385 | m1x0, m0x0 | 22 | bankc2 |
| DmaQueue_AppendFromWram | $C2838E | m0x0, m1x0 | 20 | bankc2 |
| DmaQueue_SetupYLen | $C283A2 | m1x0, m0x0 | 16 | bankc2 |
| Menu_VblankInputService | $C28473 | m1x0 | 93 | bankc2 |
| Pad_ServiceFrame | $C284D2 | m1x0 | 26 | bankc2 |
| Pad_ReadAndDecode | $C284EC | m0x0 | 89 | bankc2 |
| Menu_VblankPpuFlush | $C285D6 | m1x0 | 211 | bankc2 |
| DmaQueue_Flush | $C28663 | m0x0, m1x0 | 70 | bankc2 |
| Menu_FlushCgramBuffer | $C286A9 | m1x0, m0x0 | 52 | bankc2 |
| Menu_SetupColorMathHdma | $C286DD | m0x0, m1x0 | 148 | bankc2 |
| Menu_LoadCharRecordToBufKeepX | $C28816 | m0x0, m1x0 | 10 | bankc2 |
| Menu_LoadCharRecordToBuf | $C28839 | m1x0, m1x1 | 72 | bankc2 |
| Item_GetStatsPtr | $C288AD | m1x0 | 43 | bankc2 |
| Item_GetStatsPtr2 | $C288D8 | m1x0 | 40 | bankc2 |
| Menu_CommitComputedStats | $C29137 | m0x0, m1x0 | 55 | bankc2 |
| Menu_ComputeEquippedStats | $C2916E | m0x0, m1x0 | 109 | bankc2 |
| Sub_C291EB | $C291EB | m1x1 | 56 | bankc2 |
| Sub_C29223 | $C29223 | m1x1 | 25 | bankc2 |
| Sub_C2923C | $C2923C | m1x1 | 37 | bankc2 |
| Menu_AccumEquipSlotStats | $C29261 | m1x1 | 13 | bankc2 |
| Sub_C29276 | $C29276 | m1x1 | 62 | bankc2 |
| Sub_C29281 | $C29281 | m1x1 | 68 | bankc2 |
| Sub_C29286 | $C29286 | m1x1 | 66 | bankc2 |
| Sub_C29295 | $C29295 | m1x1 | 95 | bankc2 |
| Menu_UnpackSettingsBits | $C292F4 | m1x0 | 91 | bankc2 |
| Menu_InitPpu | $C2940D | m0x0 | 259 | bankc2 |
| Menu_ClearVramRegion | $C29512 | m1x0, m0x0 | 48 | bankc2 |
| Menu_DmaToVramFromWram | $C29542 | m0x0 | 44 | bankc2 |
| Gfx_Copy2bppTo4bppTiles | $C2960B | m0x0 | 35 | bankc2 |
| Gfx_OrPlanes012ToPlane3 | $C2962E | m0x0 | 37 | bankc2 |
| Gfx_CopyPlanes01To23 | $C29653 | m0x0 | 37 | bankc2 |
| Gfx_TileOrPlanes01ToPlane2 | $C29678 | m0x0 | 21 | bankc2 |
| PartyMenu_LoadStatsPanelGfx | $C2968D | m0x0 | 445 | bankc2 |
| Menu_ClearTilemapBuffer | $C2984A | m1x0 | 43 | bankc2 |
| Menu_BuildItemInfoTables | $C29875 | m1x0 | 275 | bankc2 |
| Menu_SanitizeInventory | $C29988 | m0x0 | 42 | bankc2 |
| Menu_RecalcAllCharStats | $C299B2 | m1x0 | 24 | bankc2 |
| Menu_BuildSaveSlotPreviews | $C2D156 | m1x0 | 66 | bankc2 |
| Menu_RenderSaveSlotLabel | $C2D296 | m1x0 | 46 | bankc2 |
| Menu_RenderSlotLabelShifted | $C2D2C4 | m1x0 | 66 | bankc2 |
| Menu_ShiftGlyphsRight2px | $C2D306 | m1x0 | 35 | bankc2 |
| NameEntry_CursorTick | $C2E267 | m1x0 | 49 | bankc2 |
| NameEntry_TypeOrBackspace | $C2E2F4 | m1x0 | 34 | bankc2 |
| NameMenu_E316 | $C2E316 | m1x0 | 52 | bankc2 |
| Menu_PositionNameEntryMarker | $C2E34A | m1x0 | 25 | bankc2 |
| NameEntry_CommitIfValid | $C2E363 | m1x0 | 45 | bankc2 |
| NameEntry_NormalizeWorkName | $C2E390 | m1x0 | 80 | bankc2 |
| NameEntry_CheckDuplicateName | $C2E3E0 | m1x0 | 46 | bankc2 |
| NameEntry_BuildGlyphGrid | $C2E545 | m1x0, m1x1 | 49 | bankc2 |
| NameEntry_LoadKeyboardGfx | $C2E576 | m1x0, m1x1 | 93 | bankc2 |
| Menu_DrawSlotDescFFCF3B | $C2E5D3 | m1x0 | 56 | bankc2 |
| Menu_ResetOamPlaceCursor | $C2E92D | m0x0 | 79 | bankc2 |
| Menu_UpdateSelectionInput | $C2E984 | m1x0 | 85 | bankc2 |
| Menu_CursorMoveFromPad | $C2E9E2 | m1x0 | 61 | bankc2 |
| Menu_CursorNodeIndex | $C2EA27 | m1x0, m1x1 | 15 | bankc2 |
| Menu_PlayCursorSfx | $C2EAC2 | m1x0 | 10 | bankc2 |
| Menu_PlayConfirmSfx | $C2EACC | m1x0 | 10 | bankc2 |
| Menu_PlaySfx | $C2EB03 | m1x1 | 28 | bankc2 |
| Menu_Draw6x6GlyphTileBlock | $C2EC38 | m1x0 | 91 | bankc2 |
| MenuOverlay_LoadWindowStylePalette | $C2ED08 | m0x0, m1x0 | 41 | bankc2 |
| Menu_RunFFUiCommandList | $C2ED31 | m1x0, m0x0 | 40 | bankc2 |
| Sub_C2ED77 | $C2ED77 | m1x0 | 25 | bankc2 |
| Menu_TilemapAddrFromRowCol | $C2ED90 | m0x0, m1x0 | 26 | bankc2 |
| Sub_C2EDAA | $C2EDAA | m1x0 | 6 | bankc2 |
| MenuOverlay_DrawWindowFrameCmd | $C2EDB0 | m1x0 | 6 | bankc2 |
| MenuOverlay_SetWindowStyleParams | $C2EDD3 | m1x0 | 35 | bankc2 |
| MenuOverlay_DrawBorderRow | $C2EE23 | m0x0 | 53 | bankc2 |
| MenuOverlay_DrawBorderSides | $C2EE58 | m0x0 | 39 | bankc2 |
| Menu_CopyTilemapRows | $C2EF2B | m0x0 | 29 | bankc2 |
| Menu_WriteGlyphTile | $C2EF97 | m1x0 | 39 | bankc2 |
| Sub_C2F332 | $C2F332 | m1x0 | 5 | bankc2 |
| Sub_C2F337 | $C2F337 | m1x0 | 43 | bankc2 |
| Sub_C2F364 | $C2F364 | m1x0 | 20 | bankc2 |
| MenuSprite_InitPartySlots | $C2F3CA | m1x0 | 88 | bankc2 |
| MenuSprite_LoadCharGfx | $C2F422 | m1x1 | 294 | bankc2 |
| MenuSprite_GetCharD1RecAddr | $C2F5ED | m1x0, m0x0 | 57 | bankc2 |
| MenuSprite_TickAnim | $C2F69D | m1x1 | 164 | bankc2 |
| Sub_C2F6D1 | $C2F6D1 | m0x0 | 1 | bankc2 |
| MenuSprite_QueueFrameVramDma | $C2F75C | m0x0 | 277 | bankc2 |
| Menu_RenderTextBlocking | $C2F90C | m0x0, m1x0 | 55 | bankc2 |
| Menu_StartDescTextRender | $C2FA49 | m1x0 | 86 | bankc2 |
| Menu_UploadRenderedText | $C2FAE9 | m1x0 | 174 | bankc2 |
| Menu_WriteTile16RowAttr31 | $C2FB97 | m1x0 | 29 | bankc2 |
| Sub_C30AFF | $C30AFF | m0x0 | 4 | bankc3 |
| Mode7Script_Dispatch | $C30B03 | m0x0 | 119 | bankc3 |
| Audio_Process_Entry | $C70004 | m1x0, m1x1 | 641 | bankc7 |
| Audio_UploadBlock2F00 | $C70922 | m1x0 | 184 | bankc7 |
| Battle_VramUploadFontTiles | $CCE22E | m1x0 | 98 | bankcc |
| Battle_InitDisplayState | $CCE2E4 | m1x0 | 2723 | bankcc |
| Battle_BuildEnemyOamTemplate | $CCED87 | m1x0 | 732 | bankcc |
| Battle_CallReencodeTextBuffer | $CCF063 | m1x0 | 8 | bankcc |
| BattleSys_BootVec | $CD0000 | m1x0 | 8 | bankcd |
| BattleSys_EnablePauseInputVec | $CD0003 | m1x0 | 8 | bankcd |
| BattleSys_PlaySong08StartFade | $CD000C | m1x0 | 28 | bankcd |
| BattleSys_PollTransitionVec | $CD0012 | m1x0 | 3 | bankcd |
| BattleMsg_ShowAndWaitVec | $CD0021 | m1x0 | 90 | bankcd |
| BattleSys_TickVector | $CD0036 | m1x0 | 7 | bankcd |
| BattleSys_Wait16Frames | $CD020A | m1x0 | 11 | bankcd |
| BattleMsg_ClearTextBufferL | $CD0235 | m1x0 | 4 | bankcd |
| BattleMsg_TickAndShow | $CD027F | m1x0 | 22 | bankcd |
| BattleSys_UpdatePadRepeat | $CD0340 | m1x0 | 89 | bankcd |
| BattleSys_HdmaSplitStepClose | $CD053C | m1x0 | 59 | bankcd |
| BattleSys_HdmaSplitStepOpen | $CD0577 | m1x0 | 59 | bankcd |
| BattleSys_TickScreenTransition | $CD05B2 | m1x0 | 77 | bankcd |
| BattleBg_NmiTilemapMsgUpload | $CD0607 | m1x0 | 171 | bankcd |
| BattleBg_BuildScrollHdmaTables | $CD06B4 | m1x0 | 8 | bankcd |
| BattleBg_ScrollHdma_Static | $CD06C4 | m1x0 | 168 | bankcd |
| Sub_CD088D | $CD088D | m1x0 | 321 | bankcd |
| BattleSys_UploadCgram | $CD09CE | m1x0 | 35 | bankcd |
| Sub_CD09F1 | $CD09F1 | m1x0 | 237 | bankcd |
| BattleBg_TickPaletteCycle | $CD0ADE | m1x0 | 16 | bankcd |
| BattleSys_SetupWindowHdma | $CD0AEE | m1x0 | 40 | bankcd |
| BattleSys_Random | $CD0B16 | m1x0 | 8 | bankcd |
| BattleSys_ClampRasterRecords | $CD0B1E | m1x0 | 51 | bankcd |
| BattleFx_UpdateScreenShake | $CD0B51 | m1x0 | 306 | bankcd |
| BattleSys_CallFieldService02 | $CD0C89 | m1x0 | 12 | bankcd |
| BattleSys_NmiDispatchByMode | $CD0C95 | m1x0 | 8 | bankcd |
| BattleSys_InitScreenFromFieldL | $CD0C9D | m1x0 | 4 | bankcd |
| BattleSys_InitScreenFromField | $CD0CA1 | m1x0 | 135 | bankcd |
| BattleScene_TickScripts | $CD1609 | m1x0 | 75 | bankcd |
| Audio_PlaySong | $CD3B71 | m1x0 | 17 | bankcd |
| BattleSys_DmaToVram | $CD3E8E | m1x0 | 38 | bankcd |
| BattleSys_RunQueuedVramDma | $CD3EB4 | m1x0 | 26 | bankcd |
| BattleSys_LoadHdmaTableSet | $CEEE6E | m1x0 | 163 | bankce |
| BattleSys_HideShadowOam | $CEF066 | m1x0 | 128 | bankce |
| BattleSys_TickUiSpriteAnims | $CFE7D7 | m1x0 | 99 | bankcf |
| BattleSys_BuildBattlerOam | $CFE83A | m1x0 | 242 | bankcf |
| BattleSys_WriteUnitMarkerSprite | $CFEA8A | m1x0 | 59 | bankcf |
| BattleSys_WriteBattlerOamEntries | $CFEAC5 | m0x0 | 207 | bankcf |
| BattleSys_CopyOamTileAttrs | $CFEB94 | m0x0 | 228 | bankcf |
| BattleSys_DrawUnitBackAnimObjects | $CFEDB8 | m1x0 | 63 | bankcf |
| BattleSys_DrawUnitFrontAnimObjects | $CFEDF7 | m1x0 | 53 | bankcf |
| BattleSys_TickMoverSlots | $CFEFAF | m1x0 | 111 | bankcf |
| BattleBg_DmaTilemapQuadrant | $D1E8D9 | m1x0 | 65 | bankd1 |
| BattleBg_FindPaletteCycleIndex | $D1EA01 | m1x0 | 32 | bankd1 |
| BattleSys_Boot | $D1F1B0 | m1x0 | 609 | bankd1 |
| BattleFx_SaveRasterRecordEnables | $D1F411 | m1x0 | 21 | bankd1 |
| BattleFx_SuspendRasterRecordsSvc | $D1F426 | m1x0 | 78 | bankd1 |
| BattleFx_InitWindowHdmaTables | $D1F83D | m1x0 | 174 | bankd1 |
| BattleSys_ApplyDisplayConfigOnChange | $D1FE4C | m1x0 | 37 | bankd1 |
| BattleSys_NmiPrimeHwMath | $D1FE71 | m1x0 | 38 | bankd1 |
| Battle_CheckUnitActBlocked | $FDA8A5 | m1x0 | 41 | bankfd |
| Loc_FDA8CE | $FDA8CE | m1x0 | 48 | bankfd |
| Battle_ResetPcPendingAction | $FDA8FE | m1x0 | 62 | bankfd |
| Loc_FDA93C | $FDA93C | m1x0 | 35 | bankfd |
| Battle_ResetFlaggedPcActions | $FDA95F | m1x0 | 35 | bankfd |
| Battle_ClearAiWorkRam | $FDA982 | m1x0 | 14 | bankfd |
| Battle_ConfusedAutoAttack | $FDAB30 | m1x0 | 114 | bankfd |
| Battle_ProcessCounterQueue | $FDAC6E | m1x0 | 128 | bankfd |
| Battle_ClearTargetResultTable | $FDACEE | m1x0 | 15 | bankfd |
| Battle_ClearWorkB328 | $FDACFD | m1x0 | 12 | bankfd |
| Battle_FillTable1A80FF | $FDAD09 | m1x0 | 14 | bankfd |
| Battle_LoadPcRecordToWork | $FDAE52 | m1x0 | 71 | bankfd |
| Battle_BuildParticipantList | $FDAE99 | m1x0 | 43 | bankfd |
| Battle_FlagPcsWithAccessoryAB | $FDAEC4 | m1x0 | 46 | bankfd |
| Battle_BuildPcTechEntries1C48 | $FDAEF2 | m1x0 | 142 | bankfd |
| Battle_AdvanceEntryCursor | $FDB01C | m1x0 | 20 | bankfd |
| Battle_GetActionRecOffset | $FDB033 | m1x0 | 162 | bankfd |
| Battle_InitUnitTimerWork | $FDB0D5 | m1x0 | 76 | bankfd |
| Battle_CopyEnemyTypeBitsToA020 | $FDB121 | m1x0 | 32 | bankfd |
| StatBoost_CheckAndApply | $FDB141 | m1x0 | 192 | bankfd |
| StatBoost_ApplyIndexed | $FDB14D | m1x0 | 180 | bankfd |
| Battle_InitSlotIndexTable | $FDB223 | m1x0 | 11 | bankfd |
| Battle_InitCombatantTables | $FDB22E | m1x0 | 176 | bankfd |
| NewGame_InitCharacterData | $FDB2DE | m1x0 | 97 | bankfd |
| CopyCharInitData | $FDB33F | m1x0 | 36 | bankfd |
| Battle_ApplyInnateStatusAndHpAccessory | $FDB363 | m1x0 | 136 | bankfd |
| Loc_FDB3EB | $FDB3EB | m1x0 | 19 | bankfd |
| Battle_InitElemModifiers | $FDB3FE | m1x0 | 58 | bankfd |
| Battle_LoadEnemyStats | $FDB438 | m1x0 | 158 | bankfd |
| Battle_ClearEnemyWorkRecord | $FDB4D6 | m1x0 | 17 | bankfd |
| Battle_InitStatusTickTimers | $FDB4E7 | m1x0 | 110 | bankfd |
| Battle_ApplyEquipEffects | $FDB555 | m1x0 | 256 | bankfd |
| Battle_InitActionWorkVars | $FDB732 | m1x0 | 185 | bankfd |
| Battle_InitAtbCounters | $FDB7EB | m1x0 | 364 | bankfd |
| Menu_RebuildTechAvail | $FFF813 | m1x0 | 318 | bankff |
| Menu_BuildColorMathHdmaTable | $FFFC04 | m1x0 | 83 | bankff |
| Field_NmiHandler | $C0EA63 | m0x0, m1x0, m1x1 | 509 | bankc0 |
| Field_IrqHandler | $C0ECCC | m0x0, m1x0 | 72 | bankc0 |
| Loc_C07F58 | $C07F58 | m1x0 | 69 | bankc0 |
| Loc_C07F5D | $C07F5D | m1x0 | 67 | bankc0 |
| ASCIITextRelatedDMAVRAM | $C0EC77 | m1x0 | 44 | bankc0 |
| AISpatial_DistSqCheck0640 | $C12AE3 | m1x0 | 127 | bankc1 |
| Battle_TickEnemySlotRoundRobin | $C1305C | m1x0 | 48 | bankc1 |
| Sub_C14058 | $C14058 | m1x0 | 358 | bankc1 |
| Battle_CountPredictedKills | $C141BE | m1x0 | 84 | bankc1 |
| BattleAnim_ResolveSlotMoveTargets | $C14212 | m1x0 | 40 | bankc1 |
| BattleAnim_TickSlotDelayTimers | $C1423A | m1x0 | 214 | bankc1 |
| BattleAnim_LoadActionConfig | $C14310 | m1x0 | 28 | bankc1 |
| Sub_C1432C | $C1432C | m1x0 | 813 | bankc1 |
| BattleAnim_ParseCEAnimHeader | $C148EC | m1x0 | 87 | bankc1 |
| BattleAnim_BuildBattlerOam | $C14943 | m1x0 | 188 | bankc1 |
| BattleAnim_SetupApproachToTarget | $C149FF | m1x0 | 114 | bankc1 |
| Battle_PathfindInit | $C14A71 | m1x0 | 333 | bankc1 |
| BattleAnim_RunActiveScripts | $C14BBE | m1x0 | 224 | bankc1 |
| Battle_AnimScriptRun | $C14C9E | m1x0 | 82 | bankc1 |
| Battle_AnimSetupBase | $C14D08 | m1x0 | 282 | bankc1 |
| BAnimCmd_03 | $C14E22 | m1x0 | 144 | bankc1 |
| BAnimCmd_04 | $C14EA2 | m1x0 | 119 | bankc1 |
| BAnimCmd_MoveBToTable | $C1542D | m1x0 | 484 | bankc1 |
| BAnimCmd_IfCounterBEq | $C15756 | m1x0 | 19 | bankc1 |
| BAnimCmd_HideAllSprites | $C15ADB | m1x0 | 534 | bankc1 |
| BAnimCmd_69 | $C15DD3 | m1x0 | 180 | bankc1 |
| BAnimCmd_A8 | $C169C3 | m1x0 | 301 | bankc1 |
| Dispatch_BankC1_ViaTable | $C1762E | m1x0 | 11 | bankc1 |
| Battle_CommitInventoryToField | $C1B4E9 | m1x0 | 140 | bankc1 |
| Battle_ResetSpriteAnimState | $CCF06B | m1x0 | 151 | bankcc |
| Battle_FillA44CWith55 | $CCF102 | m1x0 | 14 | bankcc |
| Battle_MarkBattlerGrid7CFD | $CCF110 | m1x0 | 70 | bankcc |
| Battle_AnimLoadCurAnims | $CCF156 | m1x0 | 145 | bankcc |
| Battle_TileAnimTick | $CCF1E7 | m1x0 | 145 | bankcc |
| Battle_EnemyAnimTick | $CCF278 | m1x0 | 49 | bankcc |
| Battle_EnemyAnimAdvanceFrame | $CCF2A9 | m1x0 | 188 | bankcc |
| Battle_Mul8_CC | $CCF365 | m1x0 | 30 | bankcc |
| BattleBg_LoadAnimGfxSet4Vec | $CD0015 | m1x0 | 72 | bankcd |
| BattleBg_LoadAndStartSceneVec | $CD0018 | m1x0 | 14 | bankcd |
| BattleBg_LoadAnimGfxSet2Vec | $CD002A | m1x0 | 76 | bankcd |
| BattleSys_WaitFrameFlag47 | $CD044A | m1x0 | 9 | bankcd |
| Sub_CD076C | $CD076C | m1x0 | 179 | bankcd |
| BattleScene_InitScripts | $CD0D33 | m1x0 | 47 | bankcd |
| BattleBg_StageOverlayCfgB | $CD0E79 | m1x0 | 36 | bankcd |
| BattleBg_Load | $CD0EBD | m1x0 | 405 | bankcd |
| BattleBg_FillTilemapsBlank | $CD1052 | m1x0 | 43 | bankcd |
| BattleBg_FillTilemapRegionBlank | $CD1061 | m1x0 | 28 | bankcd |
| BattleBg_BuildTilemap | $CD107D | m1x0 | 315 | bankcd |
| BattleBg_LoadDescriptor | $CD11B8 | m1x0 | 44 | bankcd |
| BattleBg_DecodeGfxChunk | $CD11E4 | m1x0 | 53 | bankcd |
| BattleBg_CopyTile | $CD12B8 | m0x0 | 72 | bankcd |
| BattleBg_DecodeAnimGfxChunk | $CD1368 | m1x0 | 98 | bankcd |
| BattleBg_GetTilePlaneMaskIdx | $CD13CA | m1x0 | 10 | bankcd |
| Sub_CD13EE | $CD13EE | m0x0 | 70 | bankcd |
| BattleBg_CopyTileVFlip2 | $CD13F6 | m0x0 | 79 | bankcd |
| BattleBg_VFlipTileRows2 | $CD13FE | m0x0 | 71 | bankcd |
| Sub_CD1445 | $CD1445 | m0x0 | 67 | bankcd |
| BattleBg_CopyTile4bpp | $CD1488 | m0x0 | 129 | bankcd |
| BattleBg_CopyAnimTile | $CD1509 | m0x0 | 204 | bankcd |
| BattleScene_RunScriptSlot | $CD1654 | m1x0 | 82 | bankcd |
| BattleSys_WaitScanline | $CD3B82 | m1x0 | 41 | bankcd |
| BattleSys_VblankDmaUploads | $CFE300 | m1x0 | 650 | bankcf |
| Battle_FrameTick | $CFE58A | m1x0 | 503 | bankcf |
| BattleSys_ApplyShakeToOam | $CFE781 | m1x0 | 86 | bankcf |
| BattleSys_StepMoveCurves | $CFEC78 | m1x0 | 74 | bankcf |
| BattleSys_UpdateAnimAnchors | $CFECC2 | m1x0 | 86 | bankcf |
| BattleSys_ShiftTrailHistory | $CFED18 | m1x0 | 160 | bankcf |
| BattleSys_DrawAnimSpriteGroup | $CFEEEB | m1x0 | 196 | bankcf |
| Sub_CFF087 | $CFF087 | m1x0 | 269 | bankcf |
| Sub_CFF61F | $CFF61F | m1x0 | 110 | bankcf |
| BattleFx_SyncLinkedPositions | $CFF9D3 | m1x0 | 36 | bankcf |
| BattleFx_SinLookup | $CFF9FB | m1x0 | 72 | bankcf |
| BattleFx_PixelToCell16 | $CFFADD | m1x0 | 5 | bankcf |
| Battle_MainBody | $CFFBE5 | m1x0 | 273 | bankcf |
| Battle_Bootstrap | $CFFB65 | m1x0 | 128 | bankcf |
| Battle_WaitVblankFrame | $CFFCFB | m1x0 | 7 | bankcf |
| Sub_C00005 | $C00005 | m1x0 | 44 | bankc0 |
| Sub_C0000B | $C0000B | m1x0 | 281 | bankc0 |
| Sub_C000EB | $C000EB | m1x0 | 32 | bankc0 |
| Scene_Unk0283 | $C00283 | m1x0, m1x1 | 550/6 | bankc0 |
| Sub_C01D5E | $C01D5E | m1x1 | 42 | bankc0 |
| Sub_C01E07 | $C01E07 | m1x0 | 161 | bankc0 |
| Sub_C01E9A | $C01E9A | m0x0 | 47 | bankc0 |
| Sub_C1006D | $C1006D | m1x0 | 5 | bankc1 |
| Sub_C128B0 | $C128B0 | m1x0 | 118 | bankc1 |
| Sub_C12926 | $C12926 | m1x0 | 96 | bankc1 |
| Sub_C129B2 | $C129B2 | m1x0 | 73 | bankc1 |
| Sub_C12AEF | $C12AEF | m1x0 | 127 | bankc1 |
| Sub_C12AFB | $C12AFB | m1x0 | 127 | bankc1 |
| Sub_C12B07 | $C12B07 | m1x0 | 127 | bankc1 |
| Sub_C12B13 | $C12B13 | m1x0 | 127 | bankc1 |
| Sub_C12B2B | $C12B2B | m1x0 | 135 | bankc1 |
| Sub_C130B6 | $C130B6 | m1x0 | 43 | bankc1 |
| Sub_C1356D | $C1356D | m1x0 | 301 | bankc1 |
| Sub_C1379A | $C1379A | m1x0 | 293 | bankc1 |
| Sub_C13A3D | $C13A3D | m1x0 | 389 | bankc1 |
| Sub_C145A0 | $C145A0 | m1x0 | 627 | bankc1 |
| Sub_C14CF0 | $C14CF0 | m1x0 | 7 | bankc1 |
| Sub_C14CF7 | $C14CF7 | m1x0 | 13 | bankc1 |
| Sub_C14D04 | $C14D04 | m1x0 | 286 | bankc1 |
| Sub_C14F1A | $C14F1A | m1x0 | 58 | bankc1 |
| Sub_C14F54 | $C14F54 | m1x0 | 107 | bankc1 |
| Sub_C1566C | $C1566C | m1x0 | 94 | bankc1 |
| Sub_C1567D | $C1567D | m1x0 | 100 | bankc1 |
| Sub_C156CC | $C156CC | m1x0 | 47 | bankc1 |
| Sub_C156FB | $C156FB | m1x0 | 25 | bankc1 |
| Sub_C15714 | $C15714 | m1x0 | 14 | bankc1 |
| Sub_C15743 | $C15743 | m1x0 | 19 | bankc1 |
| Sub_C1577D | $C1577D | m1x0 | 137 | bankc1 |
| Sub_C158DC | $C158DC | m1x0 | 8 | bankc1 |
| Sub_C158E4 | $C158E4 | m1x0 | 8 | bankc1 |
| Sub_C158EC | $C158EC | m1x0 | 18 | bankc1 |
| Sub_C1590C | $C1590C | m1x0 | 14 | bankc1 |
| Sub_C1591A | $C1591A | m1x0 | 23 | bankc1 |
| Sub_C15940 | $C15940 | m1x0 | 11 | bankc1 |
| Sub_C1594B | $C1594B | m1x0 | 11 | bankc1 |
| Sub_C1597B | $C1597B | m1x0 | 22 | bankc1 |
| Sub_C159B9 | $C159B9 | m1x0 | 43 | bankc1 |
| Sub_C15D1D | $C15D1D | m1x0 | 118 | bankc1 |
| Sub_C15E87 | $C15E87 | m1x0 | 88 | bankc1 |
| Sub_C15FC1 | $C15FC1 | m1x0 | 18 | bankc1 |
| Sub_C15FD3 | $C15FD3 | m1x0 | 11 | bankc1 |
| Sub_C15FDE | $C15FDE | m1x0 | 123 | bankc1 |
| Sub_C16059 | $C16059 | m1x0 | 54 | bankc1 |
| Sub_C160E6 | $C160E6 | m1x0 | 110 | bankc1 |
| Sub_C1619E | $C1619E | m1x0 | 35 | bankc1 |
| Sub_C161EB | $C161EB | m1x0 | 40 | bankc1 |
| Sub_C161F5 | $C161F5 | m1x0 | 38 | bankc1 |
| Sub_C1621B | $C1621B | m1x0 | 33 | bankc1 |
| Sub_C16258 | $C16258 | m1x0 | 65 | bankc1 |
| Sub_C16299 | $C16299 | m1x0 | 688 | bankc1 |
| Sub_C16549 | $C16549 | m1x0 | 113 | bankc1 |
| BattleAct_CalcMoveStep | $C165BA | m1x0 | 183 | bankc1 |
| Sub_C16AF0 | $C16AF0 | m1x0 | 513 | bankc1 |
| Sub_C16FFC | $C16FFC | m1x0 | 47 | bankc1 |
| Sub_C1702B | $C1702B | m1x0 | 47 | bankc1 |
| BattleAct_CalcFacing | $C175CC | m1x0 | 11 | bankc1 |
| BattleAct_FaceLikeEntry | $C175D7 | m1x0 | 12 | bankc1 |
| BattleAct_FaceTowardEntry | $C175E3 | m1x0 | 39 | bankc1 |
| BattleAct_CalcEntryPos | $C17639 | m1x0 | 17 | bankc1 |
| BattleAct_CalcSavedPos | $C177DA | m1x0 | 18 | bankc1 |
| BattleAct_CalcLerpToTarget | $C1780B | m1x0 | 246 | bankc1 |
| BattleAct_ProbeBoxOverlap | $C17C2B | m1x0 | 18 | bankc1 |
| BankC1_BattleStartVec | $C18000 | m1x0 | 1125 | bankc1 |
| Sub_C1895B | $C1895B | m1x0 | 26 | bankc1 |
| Sub_C19765 | $C19765 | m1x0 | 70 | bankc1 |
| Sub_C19810 | $C19810 | m1x0 | 42 | bankc1 |
| Sub_C19980 | $C19980 | m1x0 | 1 | bankc1 |
| Sub_C199B8 | $C199B8 | m1x0 | 6 | bankc1 |
| Sub_C199BE | $C199BE | m1x0 | 123 | bankc1 |
| Sub_C19A39 | $C19A39 | m1x0 | 4 | bankc1 |
| Sub_C19A3D | $C19A3D | m1x0 | 265 | bankc1 |
| Sub_C19E63 | $C19E63 | m1x0 | 21 | bankc1 |
| Sub_C1A4AF | $C1A4AF | m1x0 | 49 | bankc1 |
| Sub_C1AC89 | $C1AC89 | m1x0 | 69 | bankc1 |
| Sub_C1ACCE | $C1ACCE | m1x0 | 36 | bankc1 |
| Sub_C1ACF2 | $C1ACF2 | m1x0 | 23 | bankc1 |
| Sub_C1AF79 | $C1AF79 | m1x0 | 89 | bankc1 |
| Sub_C1B3BB | $C1B3BB | m1x0 | 23 | bankc1 |
| Sub_C1B3D2 | $C1B3D2 | m1x0 | 39 | bankc1 |
| Sub_C1B442 | $C1B442 | m1x0 | 70 | bankc1 |
| BattleAi_FindReactPart | $C1B488 | m1x0 | 34 | bankc1 |
| Sub_C1B7F2 | $C1B7F2 | m1x0 | 27 | bankc1 |
| Battle_ScaleByte20 | $C1C02A | m1x0 | 57 | bankc1 |
| Sub_C1C063 | $C1C063 | m1x0 | 55 | bankc1 |
| Sub_C1C1B2 | $C1C1B2 | m1x0 | 43 | bankc1 |
| Sub_C1D160 | $C1D160 | m1x0 | 20 | bankc1 |
| Sub_C1D174 | $C1D174 | m1x0 | 44 | bankc1 |
| Sub_C1D1A4 | $C1D1A4 | m1x0 | 17 | bankc1 |
| Sub_C1D1BB | $C1D1BB | m1x0 | 20 | bankc1 |
| Sub_C1D508 | $C1D508 | m1x0 | 27 | bankc1 |
| BattleSys_UnkD7C4 | $C1D7C4 | m1x0 | 269 | bankc1 |
| Sub_C1D8D1 | $C1D8D1 | m1x0 | 262 | bankc1 |
| Sub_C1DC64 | $C1DC64 | m1x0 | 73 | bankc1 |
| Sub_C1DE87 | $C1DE87 | m1x0 | 23 | bankc1 |
| Sub_C1DE9E | $C1DE9E | m1x0 | 82 | bankc1 |
| Sub_C1DEF0 | $C1DEF0 | m1x0 | 278 | bankc1 |
| Sub_C1E0F5 | $C1E0F5 | m1x0 | 1350 | bankc1 |
| Sub_C1E63B | $C1E63B | m1x0 | 36 | bankc1 |
| Sub_C1E65F | $C1E65F | m1x0 | 271 | bankc1 |
| Sub_C1E9B8 | $C1E9B8 | m1x0 | 29 | bankc1 |
| Sub_C1EA6F | $C1EA6F | m1x0 | 45 | bankc1 |
| Sub_C1EA9D | $C1EA9D | m1x0 | 75 | bankc1 |
| Sub_C1EAE8 | $C1EAE8 | m1x0 | 38 | bankc1 |
| BattleSys_CopyEffectBytes78 | $C1EB0E | m1x0 | 14 | bankc1 |
| Sub_C1EB1C | $C1EB1C | m1x0 | 45 | bankc1 |
| Sub_C1ED89 | $C1ED89 | m1x0 | 48 | bankc1 |
| Sub_C1F205 | $C1F205 | m1x0 | 214 | bankc1 |
| Sub_C1F2DB | $C1F2DB | m1x0 | 191 | bankc1 |
| Sub_C1F39A | $C1F39A | m1x0 | 197 | bankc1 |
| Sub_C1F45F | $C1F45F | m1x0 | 128 | bankc1 |
| Sub_C1F623 | $C1F623 | m1x0 | 406 | bankc1 |
| Sub_C1F7B9 | $C1F7B9 | m1x0 | 184 | bankc1 |
| Sub_C1F871 | $C1F871 | m1x0 | 151 | bankc1 |
| Sub_C1F908 | $C1F908 | m1x0 | 54 | bankc1 |
| Sub_C1F93E | $C1F93E | m1x0 | 172 | bankc1 |
| Sub_C1F9EB | $C1F9EB | m1x0 | 118 | bankc1 |
| BattleSys_RunPcAttackLong | $C1FDC7 | m1x0 | 4 | bankc1 |
| Battle_RandRangeLong | $C1FDCB | m1x0 | 4 | bankc1 |
| Sub_C2338F | $C2338F | m1x0 | 11 | bankc2 |
| Sub_C28349 | $C28349 | m1x0 | 4 | bankc2 |
| Sub_C30BA6 | $C30BA6 | m0x0 | 5 | bankc3 |
| Sub_C30BAA | $C30BAA | m0x0 | 1 | bankc3 |
| Sub_C30BAB | $C30BAB | m0x0 | 17 | bankc3 |
| Sub_C30BBC | $C30BBC | m0x0 | 19 | bankc3 |
| Sub_C30BC4 | $C30BC4 | m0x0 | 19 | bankc3 |
| Sub_C30C19 | $C30C19 | m0x0 | 24 | bankc3 |
| Sub_C30C41 | $C30C41 | m0x0 | 27 | bankc3 |
| Battle_InitUploadStep | $CCE000 | m1x0 | 558 | bankcc |
| Sub_CD0006 | $CD0006 | m1x0 | 153 | bankcd |
| Sub_CD001B | $CD001B | m1x0 | 8 | bankcd |
| Sub_CD0033 | $CD0033 | m1x0 | 57 | bankcd |
| Sub_CD039E | $CD039E | m1x0 | 22 | bankcd |
| Sub_CD0516 | $CD0516 | m1x0 | 30 | bankcd |
| Sub_CD0534 | $CD0534 | m1x0 | 8 | bankcd |
| Sub_CD0D62 | $CD0D62 | m1x0 | 49 | bankcd |
| Sub_CD0D93 | $CD0D93 | m1x0 | 30 | bankcd |
| Sub_CD0DB1 | $CD0DB1 | m1x0 | 73 | bankcd |
| Sub_CD0DD8 | $CD0DD8 | m1x0 | 34 | bankcd |
| Sub_CD1221 | $CD1221 | m0x0 | 70 | bankcd |
| Sub_CD1229 | $CD1229 | m0x0 | 76 | bankcd |
| Sub_CD1275 | $CD1275 | m0x0 | 67 | bankcd |
| Sub_CD13DC | $CD13DC | m1x0 | 10 | bankcd |
| Sub_CD13DE | $CD13DE | m1x0 | 8 | bankcd |
| Sub_CD13E0 | $CD13E0 | m1x0 | 6 | bankcd |
| Sub_CD13E2 | $CD13E2 | m1x0 | 4 | bankcd |
| Sub_CD15D5 | $CD15D5 | m1x0 | 52 | bankcd |
| Sub_CD16A6 | $CD16A6 | m1x0 | 15 | bankcd |
| Sub_CD1834 | $CD1834 | m1x0 | 23 | bankcd |
| Sub_CD18C2 | $CD18C2 | m1x0 | 6 | bankcd |
| Sub_CD1B42 | $CD1B42 | m1x0 | 14 | bankcd |
| Sub_CD1B50 | $CD1B50 | m1x0 | 14 | bankcd |
| Sub_CD1B5E | $CD1B5E | m1x0 | 26 | bankcd |
| Sub_CD1B6D | $CD1B6D | m1x0 | 11 | bankcd |
| Sub_CD1C32 | $CD1C32 | m1x0 | 41 | bankcd |
| Sub_CD1E07 | $CD1E07 | m1x0 | 32 | bankcd |
| Sub_CD20ED | $CD20ED | m1x0 | 6 | bankcd |
| Sub_CD2436 | $CD2436 | m1x0 | 42 | bankcd |
| Sub_CD2518 | $CD2518 | m1x0 | 35 | bankcd |
| Sub_CD29DA | $CD29DA | m1x0 | 9 | bankcd |
| Sub_CD29E3 | $CD29E3 | m1x0 | 95 | bankcd |
| Sub_CD2A4A | $CD2A4A | m1x0 | 7 | bankcd |
| Sub_CD2DCB | $CD2DCB | m1x0 | 11 | bankcd |
| Sub_CD2DD6 | $CD2DD6 | m1x0 | 20 | bankcd |
| Sub_CD2EA2 | $CD2EA2 | m1x0 | 19 | bankcd |
| Sub_CD2EB5 | $CD2EB5 | m1x0 | 19 | bankcd |
| Sub_CD2F4E | $CD2F4E | m1x0 | 19 | bankcd |
| Sub_CD2F61 | $CD2F61 | m1x0 | 13 | bankcd |
| Sub_CD2F6E | $CD2F6E | m1x0 | 29 | bankcd |
| Sub_CD2F8B | $CD2F8B | m1x0 | 32 | bankcd |
| Sub_CD2FAB | $CD2FAB | m1x0 | 6 | bankcd |
| Sub_CD2FB1 | $CD2FB1 | m1x0 | 6 | bankcd |
| Sub_CD3017 | $CD3017 | m1x0 | 9 | bankcd |
| Sub_CD3032 | $CD3032 | m1x0 | 9 | bankcd |
| Sub_CD303B | $CD303B | m1x0 | 72 | bankcd |
| Sub_CD3083 | $CD3083 | m1x0 | 9 | bankcd |
| Sub_CD308C | $CD308C | m1x0 | 6 | bankcd |
| Sub_CD3092 | $CD3092 | m1x0 | 38 | bankcd |
| Sub_CD30B8 | $CD30B8 | m1x0 | 6 | bankcd |
| BattleCF_MoveHeading | $CFF194 | m1x0 | 100 | bankcf |
| Sub_CFFCF6 | $CFFCF6 | m1x0 | 5 | bankcf |
| BattleMsg_FormatNumber4Digits | $CFFDAD | m0x0 | 115 | bankcf |
| BattleCF_RemoveMarkedEnemies | $CFFE20 | m1x0 | 101 | bankcf |
| Sub_D1F67A | $D1F67A | m1x0 | 12 | bankd1 |
| Sub_D1F74D | $D1F74D | m1x0 | 240 | bankd1 |
| Sub_D1FBCE | $D1FBCE | m1x0 | 4 | bankd1 |
| Sub_D1FCF4 | $D1FCF4 | m1x0 | 138 | bankd1 |
| BattleFD_UnkAA98 | $FDAA98 | m1x0 | 24 | bankfd |
| BattleFD_UnkAAB0 | $FDAAB0 | m1x0 | 34 | bankfd |
| BattleFD_LoadUnkB18E | $FDAAD2 | m1x0 | 47 | bankfd |
| BattleFD_LoadUnkB18E2 | $FDAB01 | m1x0 | 47 | bankfd |
| BattleFD_AddEnemyRewards | $FDABA2 | m1x0 | 204 | bankfd |
| BattleFD_UnkAD17 | $FDAD17 | m1x0 | 315 | bankfd |
| BattleFD_UnkB201 | $FDB201 | m1x0 | 34 | bankfd |
| Sub_C01DD1 | $C01DD1 | m1x1 | 39 | bankc0 |
| Sub_C01EF7 | $C01EF7 | m0x0 | 25 | bankc0 |
| Sub_CD2B45 | $CD2B45 | m1x0 | 23 | bankcd |
| Sub_CD2E13 | $CD2E13 | m1x0 | 16 | bankcd |
| Sub_CD2E4D | $CD2E4D | m1x0 | 17 | bankcd |
| Sub_CD2E78 | $CD2E78 | m1x0 | 17 | bankcd |
| Sub_C2051D | $C2051D | m1x0 | 54 | bankc2 |
| Sub_C20553 | $C20553 | m1x0 | 3 | bankc2 |
| Sub_C21807 | $C21807 | m0x0 | 18 | bankc2 |
| Sub_C21819 | $C21819 | m1x0 | 3 | bankc2 |
| Sub_C283CA | $C283CA | m1x0 | 60 | bankc2 |
| Sub_C283D0 | $C283D0 | m1x0 | 54 | bankc2 |
| Sub_C2E40E | $C2E40E | m1x0, m1x1 | 304/329 | bankc2 |
| Sub_C2EF48 | $C2EF48 | m1x0 | 29 | bankc2 |
| Sub_C2EF65 | $C2EF65 | m1x0, m0x0 | 25 | bankc2 |
| Sub_C2EF7E | $C2EF7E | m0x0, m1x0 | 25 | bankc2 |
| Sub_C2EFBE | $C2EFBE | m1x0 | 21 | bankc2 |
| Sub_C2FAA8 | $C2FAA8 | m0x0 | 68 | bankc2 |
| Sub_C0220E | $C0220E | m1x0 | 91 | bankc0 |
| Sub_C02269 | $C02269 | m1x0 | 111 | bankc0 |
| Sub_C07212 | $C07212 | m1x1 | 124 | bankc0 |
| C2Scene_TaskRunScript | $C20F63 | m1x0 | 46 | bankc2 |
| Sub_C21DD4 | $C21DD4 | m1x0 | 100 | bankc2 |
| Sub_C220A2 | $C220A2 | m1x0 | 9 | bankc2 |
| Sub_C22105 | $C22105 | m1x0 | 9 | bankc2 |
| Sub_C22F0F | $C22F0F | m1x0 | 63 | bankc2 |
| Sub_C22F94 | $C22F94 | m1x0 | 9 | bankc2 |
| Sub_C2309E | $C2309E | m1x0 | 35 | bankc2 |
| Sub_C23264 | $C23264 | m1x0 | 17 | bankc2 |
| Sub_C23404 | $C23404 | m1x0 | 41 | bankc2 |
| Sub_C23AE2 | $C23AE2 | m1x0 | 45 | bankc2 |
| Sub_C25628 | $C25628 | m1x0 | 15 | bankc2 |
| C2Scene_PlaceBelowView | $C2754D | m1x0 | 40 | bankc2 |
| Sub_C27575 | $C27575 | m1x0 | 35 | bankc2 |
| Sub_C27598 | $C27598 | m1x0 | 43 | bankc2 |
| Sub_C275C3 | $C275C3 | m1x0 | 58 | bankc2 |
| Sub_C278A1 | $C278A1 | m1x0 | 66 | bankc2 |
| Sub_C28900 | $C28900 | m1x0 | 79 | bankc2 |
| Sub_C28A98 | $C28A98 | m1x0 | 26 | bankc2 |
| Sub_C28ABA | $C28ABA | m0x0 | 27 | bankc2 |
| Sub_C28B69 | $C28B69 | m1x0 | 26 | bankc2 |
| Sub_C28B85 | $C28B85 | m1x0 | 14 | bankc2 |
| Sub_C28B93 | $C28B93 | m1x0, m0x0 | 19 | bankc2 |
| Sub_C293A8 | $C293A8 | m1x0 | 23 | bankc2 |
| Sub_C293BF | $C293BF | m1x0 | 78 | bankc2 |
| Sub_C2A0E7 | $C2A0E7 | m0x0 | 26 | bankc2 |
| Sub_C2D605 | $C2D605 | m1x0 | 19 | bankc2 |
| Sub_C2DACA | $C2DACA | m1x0 | 206 | bankc2 |
| Sub_C2DB9F | $C2DB9F | m1x0 | 73 | bankc2 |
| Sub_C2DBE8 | $C2DBE8 | m1x0 | 54 | bankc2 |
| Sub_C2DC1E | $C2DC1E | m0x0 | 86 | bankc2 |
| Sub_C2DCBE | $C2DCBE | m0x0 | 28 | bankc2 |
| Sub_C2DCDA | $C2DCDA | m1x0 | 40 | bankc2 |
| Sub_C2DD02 | $C2DD02 | m1x0 | 30 | bankc2 |
| Sub_C2DD20 | $C2DD20 | m0x0 | 32 | bankc2 |
| Sub_C2DD40 | $C2DD40 | m1x0 | 22 | bankc2 |
| Sub_C2DD56 | $C2DD56 | m0x0 | 38 | bankc2 |
| Sub_C2DD7C | $C2DD7C | m1x0 | 161 | bankc2 |
| Sub_C2DE21 | $C2DE21 | m1x0 | 53 | bankc2 |
| Sub_C2DE56 | $C2DE56 | m1x1 | 66 | bankc2 |
| Sub_C2E001 | $C2E001 | m0x0 | 17 | bankc2 |
| Sub_C2E012 | $C2E012 | m0x0 | 68 | bankc2 |
| Sub_C2E017 | $C2E017 | m1x0 | 65 | bankc2 |
| Sub_C2E058 | $C2E058 | m1x0 | 26 | bankc2 |
| Sub_C2E2A0 | $C2E2A0 | m1x0 | 34 | bankc2 |
| Sub_C2E2AA | $C2E2AA | m1x0 | 34 | bankc2 |
| Sub_C2E2CC | $C2E2CC | m1x0 | 30 | bankc2 |
| Sub_C2E2D6 | $C2E2D6 | m1x0 | 30 | bankc2 |
| Sub_C2EA36 | $C2EA36 | m1x0 | 16 | bankc2 |
| Sub_C2EA3B | $C2EA3B | m1x0 | 16 | bankc2 |
| Sub_C2EA40 | $C2EA40 | m1x0 | 16 | bankc2 |
| Sub_C2EA45 | $C2EA45 | m1x0 | 14 | bankc2 |
| Sub_C2EA53 | $C2EA53 | m1x0 | 46 | bankc2 |
| Sub_C2EA81 | $C2EA81 | m0x0, m1x0 | 57 | bankc2 |
| Sub_C2EABA | $C2EABA | m1x0 | 8 | bankc2 |
| Sub_C2EC93 | $C2EC93 | m0x0 | 25 | bankc2 |
| Sub_C2ECC2 | $C2ECC2 | m0x0 | 25 | bankc2 |
| Sub_C2EF05 | $C2EF05 | m1x0, m0x0 | 38 | bankc2 |
| Sub_C2EFF4 | $C2EFF4 | m0x0 | 17 | bankc2 |
| Sub_C2F005 | $C2F005 | m0x0 | 12 | bankc2 |
| Sub_C2F011 | $C2F011 | m0x0 | 20 | bankc2 |
| Sub_C2F022 | $C2F022 | m0x0 | 23 | bankc2 |
| Sub_C2F036 | $C2F036 | m0x0 | 53 | bankc2 |
| Sub_C2F068 | $C2F068 | m0x0 | 9 | bankc2 |
| Sub_C2F114 | $C2F114 | m0x0, m1x0 | 43 | bankc2 |
| Sub_C2F13F | $C2F13F | m1x0 | 59 | bankc2 |
| Sub_C2F1DA | $C2F1DA | m1x0 | 49 | bankc2 |
| Sub_C2F20B | $C2F20B | m1x0 | 28 | bankc2 |
| Sub_C2F2DC | $C2F2DC | m0x0 | 6 | bankc2 |
| Sub_C2F2E2 | $C2F2E2 | m0x0 | 17 | bankc2 |
| Sub_C2F2F3 | $C2F2F3 | m1x0 | 63 | bankc2 |
| Sub_C2F588 | $C2F588 | m1x0, m1x1 | 31 | bankc2 |
| Sub_C2F5A7 | $C2F5A7 | m1x0, m0x0 | 70 | bankc2 |
| Sub_C2F626 | $C2F626 | m1x0, m0x0 | 29 | bankc2 |
| Sub_C2F643 | $C2F643 | m0x0, m1x0 | 20 | bankc2 |
| Sub_C2F69B | $C2F69B | m1x1 | 166 | bankc2 |
| Sub_C2F871 | $C2F871 | m1x1 | 148 | bankc2 |
| Sub_C2FCB2 | $C2FCB2 | m1x0 | 47 | bankc2 |
| Sub_C2FD1D | $C2FD1D | m1x0 | 59 | bankc2 |
| Sub_C2FD58 | $C2FD58 | m0x0 | 46 | bankc2 |
| Sub_C2FD68 | $C2FD68 | m0x0 | 47 | bankc2 |
| Sub_C2FDBB | $C2FDBB | m1x0 | 36 | bankc2 |
| Sub_C2FDDF | $C2FDDF | m1x0 | 43 | bankc2 |
| Sub_C30008 | $C30008 | m1x0 | 338 | bankc3 |
| Sub_C301C6 | $C301C6 | m0x0 | 30 | bankc3 |
| Sub_C301CA | $C301CA | m0x0 | 26 | bankc3 |
| PalAnim_RotateDown | $FDE532 | m1x0 | 74 | bankfd |
| BankFF_BuildTileSets | $FFF548 | m0x0 | 224 | bankff |
| Sub_C220AF | $C220AF | m1x0 | 86 | bankc2 |
| Sub_C220DF | $C220DF | m1x0 | 38 | bankc2 |
| Sub_C22116 | $C22116 | m1x0 | 67 | bankc2 |
| Sub_C22152 | $C22152 | m1x0 | 36 | bankc2 |
| Sub_C2216F | $C2216F | m1x0 | 37 | bankc2 |
| Sub_C22345 | $C22345 | m0x0 | 14 | bankc2 |
| Sub_C22353 | $C22353 | m0x0 | 38 | bankc2 |
| Sub_C22379 | $C22379 | m0x0 | 47 | bankc2 |
| Sub_C22FA5 | $C22FA5 | m1x0 | 151 | bankc2 |
| Sub_C22FAE | $C22FAE | m1x0 | 142 | bankc2 |
| Sub_C230CD | $C230CD | m1x0 | 219 | bankc2 |
| Sub_C23279 | $C23279 | m1x0 | 62 | bankc2 |
| Sub_C23439 | $C23439 | m1x0 | 36 | bankc2 |
| Sub_C2563F | $C2563F | m1x0 | 310 | bankc2 |
| Sub_C25663 | $C25663 | m1x0 | 274 | bankc2 |
| Sub_C25699 | $C25699 | m1x0 | 220 | bankc2 |
| Sub_C256F9 | $C256F9 | m1x0 | 124 | bankc2 |
| Sub_CD0024 | $CD0024 | m1x0 | 202 | bankcd |
| Sub_CD00A0 | $CD00A0 | m1x0 | 106 | bankcd |
| Sub_CD010A | $CD010A | m1x0 | 147 | bankcd |
| Sub_CD0154 | $CD0154 | m1x0 | 73 | bankcd |
| Sub_CD04AA | $CD04AA | m1x0 | 53 | bankcd |
| Sub_CD0DFA | $CD0DFA | m1x0 | 11 | bankcd |
| Sub_CD0E05 | $CD0E05 | m1x0 | 30 | bankcd |
| BattleFD_AddItemEntry | $FDAF80 | m1x0 | 103 | bankfd |
| M7Scene_UpdateScroll | $FDDCBD | m1x0 | 145 | bankfd |
| PalAnim_CopyRomFrame | $FDE437 | m1x0 | 78 | bankfd |
| M7Scene_RestoreWram7F | $FDEAB7 | m1x0 | 17 | bankfd |
| M7Scene_ScreenOff | $FDEAC8 | m1x0 | 18 | bankfd |
| M7Scene_ScreenOn | $FDEADA | m1x0 | 17 | bankfd |
| M7Scene_WaitNmi | $FDEAEB | m1x0 | 9 | bankfd |
| M7Scene_BuildIris | $FDEB8B | m1x0 | 245 | bankfd |
| M7Scene_IrisEmptyLines | $FDEC80 | m1x0, m1x1 | 17 | bankfd |
| M7Scene_UploadSprGfx | $FDEC91 | m1x0 | 47 | bankfd |
| M7Scene_PlotPixel | $FDECD5 | m1x0 | 164 | bankfd |
| M7Scene_InitHdma | $FDED89 | m1x0 | 410 | bankfd |
| M7Scene_WriteScale | $FDEF23 | m1x0 | 76 | bankfd |
| M7Scene_CyclePalette | $FDF123 | m1x0 | 94 | bankfd |
| M7Scene_UploadOam | $FDF222 | m1x0 | 42 | bankfd |
| M7Scene_UploadPalette | $FDF266 | m1x0 | 42 | bankfd |
| C2Scene_NegateXVel | $C21C84 | m0x0 | 26 | bankc2 |
| Trig_Cos1024 | $C2225A | m1x0 | 11 | bankc2 |
| C2Scene_Main | $C223A8 | m1x0 | 51 | bankc2 |
| Menu_Nmi | $C2840E | m1x0 | 100 | bankc2 |
| Sub_C28820 | $C28820 | m1x1, m0x0, m1x0 | 25 | bankc2 |
| Sub_C290FC | $C290FC | m0x0, m1x0 | 37 | bankc2 |
| Menu_Mode01H0 | $C29C31 | m1x0 | 94 | bankc2 |
| Sub_C29E76 | $C29E76 | m0x0 | 58 | bankc2 |
| Menu_Icon0Preview | $C2A051 | m1x0 | 150 | bankc2 |
| Sub_C2A170 | $C2A170 | m1x0 | 66 | bankc2 |
| Sub_C2A1EF | $C2A1EF | m0x0, m1x0 | 39 | bankc2 |
| Sub_C2A216 | $C2A216 | m0x0 | 9 | bankc2 |
| Menu_LoadWindowGfx | $C2C511 | m1x0 | 55 | bankc2 |
| Sub_C2C649 | $C2C649 | m1x0, m1x1 | 21 | bankc2 |
| Sub_C2C805 | $C2C805 | m0x0 | 86 | bankc2 |
| Sub_C2C85B | $C2C85B | m1x1 | 95 | bankc2 |
| Sub_C2C8BA | $C2C8BA | m0x0 | 143 | bankc2 |
| Sub_C2C9E5 | $C2C9E5 | m1x1, m1x0 | 126 | bankc2 |
| Sub_C2CAF3 | $C2CAF3 | m1x0 | 152 | bankc2 |
| Sub_C2CB8B | $C2CB8B | m1x1 | 39 | bankc2 |
| Sub_C2CC4F | $C2CC4F | m1x0, m0x0 | 100 | bankc2 |
| Sub_C2CD2B | $C2CD2B | m0x0 | 112 | bankc2 |
| Sub_C2CD9B | $C2CD9B | m1x1 | 48 | bankc2 |
| Sub_C2CE86 | $C2CE86 | m1x0 | 16 | bankc2 |
| Sub_C2CFFB | $C2CFFB | m1x0 | 73 | bankc2 |
| Sub_C2D0E5 | $C2D0E5 | m1x0 | 40 | bankc2 |
| Menu_MainCursorThread | $C2E91B | m0x0 | 8 | bankc2 |
| Sub_C2E923 | $C2E923 | m1x0 | 50 | bankc2 |
| Menu_DrawText4bpp | $C2F905 | m1x0 | 57 | bankc2 |
| Sub_FFF677 | $FFF677 | m1x0 | 123 | bankff |
| Sub_C2A1B2 | $C2A1B2 | m1x0 | 55 | bankc2 |
| Sub_C2A22F | $C2A22F | m1x0 | 68 | bankc2 |
| Sub_C2A273 | $C2A273 | m1x0 | 83 | bankc2 |
| Sub_C2A6F0 | $C2A6F0 | m1x0 | 46 | bankc2 |
| Sub_C2CCDB | $C2CCDB | m0x0 | 80 | bankc2 |
| Sub_C2F28D | $C2F28D | m1x0, m0x0 | 63 | bankc2 |
| Sub_C2F2CC | $C2F2CC | m1x0 | 16 | bankc2 |
| Sub_C07217 | $C07217 | m1x1 | 124 | bankc0 |
| BattleTgt_RunAreaQuery | $C11FDD | m1x0 | 13 | bankc1 |
| AISpatial01_FarthestBattler | $C129FB | m1x0 | 72 | bankc1 |
| AISpatial03_FarthestEnemy | $C12A98 | m1x0 | 75 | bankc1 |
| AISpatial_DistSqCheck0100 | $C12B1F | m1x0 | 127 | bankc1 |
| AISpatial05_YDistGT32 | $C12BBC | m1x0 | 30 | bankc1 |
| AISpatial06_IsAbove | $C12BDA | m1x0 | 20 | bankc1 |
| AISpatial07_IsLeftOf | $C12BEE | m1x0 | 20 | bankc1 |
| AISpatial08_GeomTest | $C12C02 | m1x0 | 165 | bankc1 |
| AISpatial09_InTopHalf | $C12CA7 | m1x0 | 19 | bankc1 |
| AISpatial0A_InBottomHalf | $C12CBA | m1x0 | 19 | bankc1 |
| AISpatial0B_InLeftArea | $C12CCD | m1x0 | 19 | bankc1 |
| AISpatial0C_InRightArea | $C12CE0 | m1x0 | 19 | bankc1 |
| AISpatial0D_PosCompare | $C12CF3 | m1x0 | 142 | bankc1 |
| Sub_C130E1 | $C130E1 | m1x0 | 96 | bankc1 |
| Sub_C13141 | $C13141 | m1x0 | 34 | bankc1 |
| Sub_C13163 | $C13163 | m1x0 | 34 | bankc1 |
| Sub_C13185 | $C13185 | m1x0 | 12 | bankc1 |
| Sub_C13191 | $C13191 | m1x0 | 31 | bankc1 |
| Sub_C131B0 | $C131B0 | m1x0 | 34 | bankc1 |
| Sub_C131D2 | $C131D2 | m1x0 | 34 | bankc1 |
| Sub_C131F4 | $C131F4 | m1x0 | 34 | bankc1 |
| Sub_C1354D | $C1354D | m1x0 | 221 | bankc1 |
| Loc_C13772 | $C13772 | m1x0 | 40 | bankc1 |
| Loc_C138BF | $C138BF | m1x0 | 154 | bankc1 |
| Loc_C13959 | $C13959 | m1x0 | 228 | bankc1 |
| Loc_C13BC2 | $C13BC2 | m1x0 | 625 | bankc1 |
| Loc_C13E33 | $C13E33 | m1x0 | 295 | bankc1 |
| Loc_C13F5A | $C13F5A | m1x0 | 199 | bankc1 |
| Loc_C14021 | $C14021 | m1x0 | 55 | bankc1 |
| Sub_C1475A | $C1475A | m1x0 | 1 | bankc1 |
| Sub_C1475B | $C1475B | m1x0 | 401 | bankc1 |
| BAnimCmd_MoveToCoords | $C14FBF | m1x0 | 346 | bankc1 |
| BAnimCmd_MoveToTarget | $C14FD1 | m1x0 | 346 | bankc1 |
| BAnimCmd_MoveToTableCoords | $C14FE3 | m1x0 | 338 | bankc1 |
| BattleAnim_InitMoveToTarget | $C15135 | m1x0 | 185 | bankc1 |
| BAnimCmd_MoveToCoordsSpd | $C151EE | m1x0 | 412 | bankc1 |
| BAnimCmd_MoveToTargetSpd | $C15205 | m1x0 | 412 | bankc1 |
| BAnimCmd_MoveToTableSpd | $C1521C | m1x0 | 404 | bankc1 |
| Loc_C153B0 | $C153B0 | m1x0 | 89 | bankc1 |
| BAnimCmd_MoveBToCoords | $C15409 | m1x0 | 492 | bankc1 |
| BAnimCmd_MoveBToTarget | $C1541B | m1x0 | 492 | bankc1 |
| BAnimCmd_StepToCoords | $C15611 | m1x0 | 91 | bankc1 |
| BAnimCmd_1C | $C15694 | m1x0 | 39 | bankc1 |
| BAnimCmd_HideSpriteSlot | $C156BB | m1x0 | 17 | bankc1 |
| BAnimCmd_WaitTargetFlag | $C15722 | m1x0 | 35 | bankc1 |
| BAnimCmd_IfVarEq | $C1572C | m1x0 | 23 | bankc1 |
| BAnimCmd_EndIfBattlerGone | $C15769 | m1x0 | 20 | bankc1 |
| BAnimCmd_27 | $C15806 | m1x0 | 58 | bankc1 |
| BAnimCmd_28 | $C15840 | m1x0 | 58 | bankc1 |
| BAnimCmd_29 | $C15844 | m1x0 | 53 | bankc1 |
| BAnimCmd_2A | $C15879 | m1x0 | 53 | bankc1 |
| BAnimCmd_2B | $C1587D | m1x0 | 46 | bankc1 |
| BAnimCmd_SignalReady | $C158AB | m1x0 | 49 | bankc1 |
| BAnimCmd_SetCounterA | $C158FE | m1x0 | 14 | bankc1 |
| BAnimCmd_IncVar | $C15931 | m1x0 | 15 | bankc1 |
| BAnimCmd_DecVar | $C15956 | m1x0 | 15 | bankc1 |
| BAnimCmd_DecCounterA | $C15965 | m1x0 | 11 | bankc1 |
| BAnimCmd_DecCounterB | $C15970 | m1x0 | 11 | bankc1 |
| BAnimCmd_AddCounterAImm | $C15991 | m1x0 | 18 | bankc1 |
| BAnimCmd_AddCounterAVar | $C159A3 | m1x0 | 22 | bankc1 |
| BAnimCmd_CopyVar | $C159E4 | m1x0 | 22 | bankc1 |
| BAnimCmd_VarFromCounterA | $C159FA | m1x0 | 18 | bankc1 |
| BAnimCmd_43 | $C15A0C | m1x0 | 50 | bankc1 |
| BAnimCmd_44 | $C15A3E | m1x0 | 50 | bankc1 |
| BAnimCmd_49 | $C15A70 | m1x0 | 41 | bankc1 |
| BAnimCmd_SwapCounters | $C15A99 | m1x0 | 22 | bankc1 |
| BAnimCmd_SwapCounterATargetX | $C15AAF | m1x0 | 22 | bankc1 |
| BAnimCmd_SwapCounterATargetY | $C15AC5 | m1x0 | 22 | bankc1 |
| BAnimCmd_5D | $C15CF1 | m1x0 | 33 | bankc1 |
| BAnimCmd_5E | $C15D12 | m1x0 | 37 | bankc1 |
| BAnimCmd_Nop2 | $C15D18 | m1x0 | 5 | bankc1 |
| BAnimCmd_StartEffectChannel | $C15D93 | m1x0 | 46 | bankc1 |
| BAnimCmd_StopEffectChannel | $C15DC1 | m1x0 | 18 | bankc1 |
| BAnimCmd_ScreenFlashShake | $C15EDF | m1x0 | 52 | bankc1 |
| BAnimCmd_6C | $C15F13 | m1x0 | 386 | bankc1 |
| BAnimCmd_StopFlashShake | $C15F9A | m1x0 | 11 | bankc1 |
| BAnimCmd_PauseAllSprites | $C15FA5 | m1x0 | 14 | bankc1 |
| BAnimCmd_ResumeAllSprites | $C15FB3 | m1x0 | 14 | bankc1 |
| BAnimCmd_74 | $C1605F | m1x0 | 52 | bankc1 |
| BAnimCmd_75 | $C16093 | m1x0 | 83 | bankc1 |
| BAnimCmd_77 | $C16154 | m1x0 | 74 | bankc1 |
| BAnimCmd_PlaySfxAtCoords | $C161C1 | m1x0 | 42 | bankc1 |
| BAnimCmd_47 | $C1623C | m1x0 | 28 | bankc1 |
| Battle_MoveLineInitVariant | $C16671 | m1x0 | 189 | bankc1 |
| BAnimCmd_99 | $C1672E | m1x0 | 692 | bankc1 |
| BAnimCmd_9A | $C16743 | m1x0 | 693 | bankc1 |
| BAnimCmd_9B | $C1674A | m1x0 | 697 | bankc1 |
| BAnimCmd_A2 | $C16751 | m1x0 | 458 | bankc1 |
| BAnimCmd_A4 | $C1691B | m1x0 | 159 | bankc1 |
| BAnimCmd_A5 | $C169BA | m1x0 | 152 | bankc1 |
| BAnimCmd_C0 | $C16CF3 | m1x0 | 574 | bankc1 |
| BAnimCmd_C1 | $C16F31 | m1x0 | 579 | bankc1 |
| BAnimCmd_C2 | $C16F53 | m1x0 | 575 | bankc1 |
| BAnimCmd_C3 | $C16F75 | m1x0 | 578 | bankc1 |
| BAnimCmd_C4 | $C16F9A | m1x0 | 85 | bankc1 |
| BAnimCmd_C5 | $C16FEF | m1x0 | 84 | bankc1 |
| BAnimCmd_D2 | $C1705A | m1x0 | 436 | bankc1 |
| BAnimCmd_TargetFromStoredPoint | $C1720E | m1x0 | 261 | bankc1 |
| Battle_DivideMajorLenBy | $C17313 | m1x0 | 54 | bankc1 |
| BAnimCmd_D4 | $C17349 | m1x0 | 100 | bankc1 |
| BAnimCmd_D6 | $C17364 | m1x0 | 73 | bankc1 |
| BAnimCmd_WaitTimer16 | $C173AD | m1x0 | 94 | bankc1 |
| BAnimCmd_D7 | $C1740B | m1x0 | 93 | bankc1 |
| Battle_ClampActionTargetByRange | $C17468 | m1x0 | 76 | bankc1 |
| BAnimCmd_D8 | $C174B4 | m1x0 | 134 | bankc1 |
| BAnimCmd_D9 | $C1753A | m1x0 | 63 | bankc1 |
| BAnimCmd_DA | $C17579 | m1x0 | 66 | bankc1 |
| BattleAct_FaceFixed | $C1760A | m1x0 | 8 | bankc1 |
| BattleAct_FaceTowardCentre | $C17612 | m1x0 | 28 | bankc1 |
| BattleAct_CalcEntryOffsetPos | $C1764A | m1x0 | 51 | bankc1 |
| BattleAct_CalcNone | $C1767D | m1x0 | 1 | bankc1 |
| BattleAct_CalcMid03 | $C1767E | m1x0 | 77 | bankc1 |
| BattleAct_CalcMid01 | $C17686 | m1x0 | 77 | bankc1 |
| BattleAct_CalcMid02 | $C1768E | m1x0 | 74 | bankc1 |
| BattleAct_CalcCentroid | $C176D8 | m1x0 | 118 | bankc1 |
| BattleAct_CalcAheadOfActor | $C1774E | m1x0 | 131 | bankc1 |
| BattleAct_CalcFixedPoint | $C177D1 | m1x0 | 9 | bankc1 |
| BattleAct_CalcObjOffset | $C177EC | m1x0 | 31 | bankc1 |
| BattleAct_CalcVarPair | $C17901 | m1x0 | 19 | bankc1 |
| BattleAct_CalcMid12 | $C17914 | m1x0 | 77 | bankc1 |
| BattleAct_CalcLerpToObj0 | $C17920 | m1x0 | 129 | bankc1 |
| Sub_C18876 | $C18876 | m1x0 | 79 | bankc1 |
| Sub_C188C5 | $C188C5 | m1x0 | 150 | bankc1 |
| Sub_C18975 | $C18975 | m1x0 | 68 | bankc1 |
| Sub_C189B9 | $C189B9 | m1x0 | 76 | bankc1 |
| Sub_C18A51 | $C18A51 | m1x0 | 76 | bankc1 |
| Sub_C18A9D | $C18A9D | m1x0 | 1 | bankc1 |
| Sub_C18A9E | $C18A9E | m1x0 | 1 | bankc1 |
| Sub_C18A9F | $C18A9F | m1x0 | 113 | bankc1 |
| Sub_C18B10 | $C18B10 | m1x0 | 169 | bankc1 |
| Sub_C18BB9 | $C18BB9 | m1x0 | 79 | bankc1 |
| Sub_C18C08 | $C18C08 | m1x0 | 1 | bankc1 |
| PointersToEn_C18EAB | $C18EAB | m1x0 | 102 | bankc1 |
| PointersToEn_C18F11 | $C18F11 | m1x0 | 118 | bankc1 |
| PointersToEn_C18F87 | $C18F87 | m1x0 | 83 | bankc1 |
| PointersToEn_C18FDA | $C18FDA | m1x0 | 57 | bankc1 |
| PointersToEn_C19013 | $C19013 | m1x0 | 50 | bankc1 |
| AICond06_BattleTimeGE | $C19045 | m1x0 | 61 | bankc1 |
| PointersToEn_C19082 | $C19082 | m1x0 | 60 | bankc1 |
| AICond08_TargetHpLE | $C190BE | m1x0 | 114 | bankc1 |
| AICond09_TargetFieldGE | $C19130 | m1x0 | 94 | bankc1 |
| AICond0A_TargetFieldBits | $C1918E | m1x0 | 107 | bankc1 |
| PointersToEn_C191F9 | $C191F9 | m1x0 | 100 | bankc1 |
| PointersToEn_C1925D | $C1925D | m1x0 | 70 | bankc1 |
| AICond0D_TargetCheckReq5 | $C192A3 | m1x0 | 113 | bankc1 |
| AICond0E_TargetCheckReq | $C19314 | m1x0 | 121 | bankc1 |
| AICond0F_TargetCheckReq8 | $C1938D | m1x0 | 89 | bankc1 |
| PointersToEn_C193E6 | $C193E6 | m1x0 | 68 | bankc1 |
| AICond11_ActorStateCmp | $C1942A | m1x0 | 74 | bankc1 |
| PointersToEn_C19474 | $C19474 | m1x0 | 94 | bankc1 |
| AICond13_ActorStateBit10 | $C194D2 | m1x0 | 66 | bankc1 |
| AICond14_ActorTargetCmp | $C19514 | m1x0 | 134 | bankc1 |
| PointersToEn_C1959A | $C1959A | m1x0 | 60 | bankc1 |
| PointersToEn_C195D6 | $C195D6 | m1x0 | 4 | bankc1 |
| PointersToEn_C195DA | $C195DA | m1x0 | 32 | bankc1 |
| PointersToEn_C195FA | $C195FA | m1x0 | 88 | bankc1 |
| PointersToEn_C19652 | $C19652 | m1x0 | 4 | bankc1 |
| PointersToEn_C19656 | $C19656 | m1x0 | 79 | bankc1 |
| PointersToEn_C196A5 | $C196A5 | m1x0 | 47 | bankc1 |
| AICond1C_TargetIsPC | $C196D4 | m1x0 | 84 | bankc1 |
| AICond1D_TargetSlotEmpty | $C19728 | m1x0 | 52 | bankc1 |
| AICond1E_AlwaysPassStop | $C1975C | m1x0 | 9 | bankc1 |
| PointersToEn_C197AB | $C197AB | m1x0 | 21 | bankc1 |
| AICond21_ActorAF15Flag | $C197C0 | m1x0 | 21 | bankc1 |
| AICond22_TargetB158Cmp | $C197D5 | m1x0 | 59 | bankc1 |
| PointersToEn_C198C4 | $C198C4 | m1x0 | 1 | bankc1 |
| PointersToEn_C198C5 | $C198C5 | m1x0 | 249 | bankc1 |
| PointersToEn_C19960 | $C19960 | m1x0 | 1 | bankc1 |
| PointersToEn_C19961 | $C19961 | m1x0 | 1 | bankc1 |
| PointersToEn_C19962 | $C19962 | m1x0 | 4 | bankc1 |
| PointersToEn_C19966 | $C19966 | m1x0 | 1 | bankc1 |
| PointersToEn_C19967 | $C19967 | m1x0 | 17 | bankc1 |
| PointersToEn_C19978 | $C19978 | m1x0 | 1 | bankc1 |
| PointersToEn_C19979 | $C19979 | m1x0 | 4 | bankc1 |
| PointersToEn_C1997D | $C1997D | m1x0 | 1 | bankc1 |
| PointersToEn_C1997E | $C1997E | m1x0 | 1 | bankc1 |
| PointersToEn_C1997F | $C1997F | m1x0 | 1 | bankc1 |
| AIPrep10_CountFreeSlots | $C19981 | m1x0 | 51 | bankc1 |
| PointersToEn_C199B4 | $C199B4 | m1x0 | 4 | bankc1 |
| AIAct03_Nop | $C19B46 | m1x0 | 1 | bankc1 |
| AIAct04_Nop | $C19B47 | m1x0 | 1 | bankc1 |
| AIAct05_RepeatOp | $C19B48 | m1x0 | 68 | bankc1 |
| AIAct06_Nop | $C19B8C | m1x0 | 1 | bankc1 |
| AIAct07_Transform | $C19B8D | m1x0 | 225 | bankc1 |
| AIAct08_Nop | $C19C6E | m1x0 | 1 | bankc1 |
| AIAct09_RemoveSelf | $C19C6F | m1x0 | 68 | bankc1 |
| AIAct0A_FleeMsg | $C19CB3 | m1x0 | 104 | bankc1 |
| AIAct0B_SetOwnField | $C19D1B | m1x0 | 87 | bankc1 |
| AIAct0C_FieldOpMsg | $C19D72 | m1x0 | 92 | bankc1 |
| AIAct0D_SelfStatusFlags | $C19DCE | m1x0 | 148 | bankc1 |
| AIAct0E_Nop | $C19E62 | m1x0 | 1 | bankc1 |
| AIAct10_TargetsOpMsg | $C19E78 | m1x0 | 226 | bankc1 |
| AIAct11_SetOwnFieldsDirect | $C19F5A | m1x0 | 120 | bankc1 |
| AIAct12_MultiOpFMsg | $C19FD2 | m1x0 | 380 | bankc1 |
| AIAct13_ModTargetB158 | $C1A14E | m1x0 | 58 | bankc1 |
| AIAct14_AddSubOwnFields | $C1A188 | m1x0 | 131 | bankc1 |
| AIAct15_AddFieldsCoopTech | $C1A20B | m1x0 | 395 | bankc1 |
| AIAct16_SummonAll | $C1A396 | m1x0 | 59 | bankc1 |
| EnemyAI_AddSubOwnField | $C1A3D1 | m1x0 | 37 | bankc1 |
| BattleAi_TargetNone | $C1A3F6 | m1x0 | 1 | bankc1 |
| AITgt01_AllPCs | $C1A3F7 | m1x0 | 26 | bankc1 |
| AITgt02_AllCombatants | $C1A411 | m1x0 | 29 | bankc1 |
| AITgt03_Self | $C1A42E | m1x0 | 15 | bankc1 |
| AITgt04_CurTarget | $C1A43D | m1x0 | 21 | bankc1 |
| AITgt07_FarthestBattler | $C1A4E0 | m1x0 | 40 | bankc1 |
| AITgt08_LowestHpPC | $C1A508 | m1x0 | 57 | bankc1 |
| AITgt09_StatField1D | $C1A541 | m1x0 | 10 | bankc1 |
| Sub_C1A54B | $C1A54B | m1x0 | 10 | bankc1 |
| Sub_C1A555 | $C1A555 | m1x0 | 10 | bankc1 |
| Sub_C1A55F | $C1A55F | m1x0 | 10 | bankc1 |
| Sub_C1A569 | $C1A569 | m1x0 | 10 | bankc1 |
| AITgt0E_PCsStatus1E_02 | $C1A573 | m1x0 | 48 | bankc1 |
| AITgt0F_PCsStatus1E_80 | $C1A5A3 | m1x0 | 48 | bankc1 |
| AITgt10_PCsStatus1E_04 | $C1A5D3 | m1x0 | 48 | bankc1 |
| AITgt11_PCsStatus21_04 | $C1A603 | m1x0 | 48 | bankc1 |
| AITgt12_PCsStatus21_40 | $C1A633 | m1x0 | 48 | bankc1 |
| AITgt13_PCsStatus20_10 | $C1A663 | m1x0 | 48 | bankc1 |
| AITgt14_PCsField01_08 | $C1A693 | m1x0 | 48 | bankc1 |
| AITgt15_OtherEnemies | $C1A6C3 | m1x0 | 42 | bankc1 |
| AITgt16_AllEnemies | $C1A6ED | m1x0 | 28 | bankc1 |
| AITgt17_NearestEnemy | $C1A709 | m1x0 | 46 | bankc1 |
| AITgt18_FarthestEnemy | $C1A737 | m1x0 | 46 | bankc1 |
| AITgt19_LowestHpEnemy | $C1A765 | m1x0 | 68 | bankc1 |
| AITgt1A_StatusedEnemiesNoSelf | $C1A7A9 | m1x0 | 60 | bankc1 |
| AITgt1B_StatusedEnemies | $C1A7E5 | m1x0 | 52 | bankc1 |
| Sub_C1A819 | $C1A819 | m1x0 | 60 | bankc1 |
| Sub_C1A855 | $C1A855 | m1x0 | 52 | bankc1 |
| Sub_C1A889 | $C1A889 | m1x0 | 60 | bankc1 |
| Sub_C1A8C5 | $C1A8C5 | m1x0 | 52 | bankc1 |
| Sub_C1A8F9 | $C1A8F9 | m1x0 | 60 | bankc1 |
| Sub_C1A935 | $C1A935 | m1x0 | 60 | bankc1 |
| Sub_C1A971 | $C1A971 | m1x0 | 60 | bankc1 |
| Sub_C1A9AD | $C1A9AD | m1x0 | 60 | bankc1 |
| Sub_C1A9E9 | $C1A9E9 | m1x0 | 60 | bankc1 |
| Sub_C1AA25 | $C1AA25 | m1x0 | 60 | bankc1 |
| Sub_C1AA61 | $C1AA61 | m1x0 | 74 | bankc1 |
| Sub_C1AAAB | $C1AAAB | m1x0 | 11 | bankc1 |
| Sub_C1AAB6 | $C1AAB6 | m1x0 | 11 | bankc1 |
| Sub_C1AAC1 | $C1AAC1 | m1x0 | 11 | bankc1 |
| Sub_C1AACC | $C1AACC | m1x0 | 11 | bankc1 |
| Sub_C1AAD7 | $C1AAD7 | m1x0 | 11 | bankc1 |
| Sub_C1AAE2 | $C1AAE2 | m1x0 | 11 | bankc1 |
| Sub_C1AAED | $C1AAED | m1x0 | 11 | bankc1 |
| Sub_C1AAF8 | $C1AAF8 | m1x0 | 11 | bankc1 |
| Sub_C1AB03 | $C1AB03 | m1x0 | 75 | bankc1 |
| Sub_C1AB4E | $C1AB4E | m1x0 | 11 | bankc1 |
| Sub_C1AB59 | $C1AB59 | m1x0 | 11 | bankc1 |
| Sub_C1AB64 | $C1AB64 | m1x0 | 11 | bankc1 |
| Sub_C1AB6F | $C1AB6F | m1x0 | 11 | bankc1 |
| Sub_C1AB7A | $C1AB7A | m1x0 | 11 | bankc1 |
| Sub_C1AB85 | $C1AB85 | m1x0 | 11 | bankc1 |
| Sub_C1AB90 | $C1AB90 | m1x0 | 11 | bankc1 |
| Sub_C1AB9B | $C1AB9B | m1x0 | 46 | bankc1 |
| Sub_C1ABC9 | $C1ABC9 | m1x0 | 75 | bankc1 |
| EnemyAI_RunTargetOp | $C1AC2C | m1x0 | 26 | bankc1 |
| BattleAi_TargetsByPcByte | $C1AD68 | m1x0 | 57 | bankc1 |
| Loc_C1ADA1 | $C1ADA1 | m1x0 | 122 | bankc1 |
| BattleAi_PickMarkedTarget | $C1AE21 | m1x0 | 79 | bankc1 |
| BattleAi_AddPcByByte | $C1AE70 | m1x0 | 99 | bankc1 |
| BattleAi_NoteFirstTest | $C1AEFD | m1x0 | 37 | bankc1 |
| Sub_C2F6CE | $C2F6CE | m0x0 | 4 | bankc2 |
| Sub_C2F6D2 | $C2F6D2 | m0x0 | 2 | bankc2 |
| Sub_C2F6D4 | $C2F6D4 | m0x0 | 4 | bankc2 |
| BattleFx_StopScreenShake | $D1F692 | m1x0 | 10 | bankd1 |
| Loc_D1F6B4 | $D1F6B4 | m1x0 | 153 | bankd1 |
| BattleFD_RestoreEnemies | $FDA990 | m1x0 | 264 | bankfd |
| Tech_Learn | $C1F4DF | m1x0 | 84 | bankc1 |
| Sub_C2116B | $C2116B | m0x0 | 71 | bankc2 |
| Sub_C21469 | $C21469 | m0x0 | 34 | bankc2 |
| Menu_Sin | $C2225E | m0x0 | 21 | bankc2 |
| Loc_C2338E | $C2338E | m1x0 | 1 | bankc2 |
| Sub_C234F8 | $C234F8 | m1x0 | 634 | bankc2 |
| Sub_C23567 | $C23567 | m1x0 | 523 | bankc2 |
| Sub_C236ED | $C236ED | m1x0 | 133 | bankc2 |
| Sub_C275FD | $C275FD | m1x0 | 85 | bankc2 |
| Sub_C27652 | $C27652 | m1x0 | 176 | bankc2 |
| Sub_C27BC4 | $C27BC4 | m1x0 | 13 | bankc2 |
| Spr_Place24 | $C0C73A | m1x0 | 592 | bankc0 |
| Spr_CopyTileD4 | $C0E739 | m1x0 | 131 | bankc0 |
| Spr_CopyTileD2 | $C0E7BC | m1x0 | 131 | bankc0 |
| Spr_CopyTileD3 | $C0E83F | m1x0 | 131 | bankc0 |
| Spr_CopyTile7F | $C0E8C2 | m1x0 | 115 | bankc0 |
| MusicCueDispatch | $C01BAB | m1x0 | 59 | bankc0 |
| Field_RunBankC2Mode5 | $C019C7 | m1x0 | 11 | bankc0 |
| ModeE6_Handler | $C00D78 | m1x0 | 772 | bankc0 |
| ModeEC_Handler | $C00E5F | m1x0 | 978 | bankc0 |
| ModeEE_Handler | $C01014 | m1x0 | 978 | bankc0 |
| ModeFA_Handler | $C011C9 | m1x0 | 1192 | bankc0 |
| ModeFC_Handler | $C01454 | m1x0 | 1189 | bankc0 |
| Obj_BuildFrameLayout | $C0CB0A | m1x1 | 1733 | bankc0 |
| Obj_BuildFrame4 | $C0CBDC | m1x1 | 492 | bankc0 |
| Obj_FrameLayout4 | $C0CDC8 | m1x1 | 301 | bankc0 |
| Obj_BuildFrame8 | $C0CEF5 | m1x1 | 1180 | bankc0 |
| Obj_FrameLayout8 | $C0D124 | m1x1 | 358 | bankc0 |
| Obj_BuildFrame8Pass0 | $C0D28A | m1x1 | 131 | bankc0 |
| Obj_BuildFrame8Pass1 | $C0D30D | m1x1 | 490 | bankc0 |
| Obj_BuildFrame12 | $C0D4F7 | m1x1 | 2097 | bankc0 |
| Obj_BuildFrame12Pass0Alt | $C0D608 | m1x1 | 131 | bankc0 |
| Obj_BuildFrame12Pass1 | $C0D68B | m1x1 | 173 | bankc0 |
| Obj_BuildFrame12Pass1Alt | $C0D738 | m1x1 | 173 | bankc0 |
| Obj_BuildFrame12Pass2 | $C0D7E5 | m1x1 | 644 | bankc0 |
| Obj_BuildFrame12Pass2Alt | $C0DA69 | m1x1 | 703 | bankc0 |
| Obj_FrameLayout12 | $C0DD28 | m1x1 | 1026 | bankc0 |
| BattleAct_AdvanceScript | $C175BB | m1x0 | 17 | bankc1 |
| BankC1_OldBuildLeftovers | $C17C3D | m1x0 | 10 | bankc1 |
| BattleAi_UnkAED3 | $C1AED3 | m1x0 | 42 | bankc1 |
| BattleSys_EffRecArgsUnk66 | $C1D132 | m1x0 | 29 | bankc1 |
| BattleSys_LoadEffectArgs | $C1D1D3 | m1x0 | 38 | bankc1 |
| BattleSys_Effect00 | $C1D221 | m1x0 | 29 | bankc1 |
| BattleSys_EffectNone | $C1D48F | m1x0 | 1 | bankc1 |
| BattleSys_CasterHitOffset | $C1E8C0 | m1x0 | 33 | bankc1 |
| Battle_SumHitSets | $C1EB68 | m1x0 | 144 | bankc1 |
| BankC1_AddItemLong | $C1FDC3 | m1x0 | 4 | bankc1 |
| BankC1_OldBuildTail | $C1FDD3 | m1x0 | 3 | bankc1 |
| C3_BootIrq | $C30529 | m1x0 | 31 | bankc3 |
| WinFx_BuildQuad | $C30EFA | m1x0 | 59 | bankc3 |
| BankC3_PackedModule | $C31313 | m1x0 | 13 | bankc3 |
| Sub_C33063 | $C33063 | m1x0 | 12 | bankc3 |
| C6Scene_Win1HdmaOn | $C6E000 | m1x0 | 40 | bankc6 |
| BattleCE_CopyWindowEdges | $CEE000 | m1x0 | 142 | bankce |
| BattleCF_DrawPosHistory | $CFE92C | m1x0 | 112 | bankcf |
| BattleCF_DrawPosHistRecord | $CFE99C | m1x0 | 238 | bankcf |
| BattleCF_DrawFxOverlay | $CFEE2C | m1x0 | 115 | bankcf |
| BattleCF_DrawHitNumbers | $CFEE9F | m1x0 | 76 | bankcf |
| BattleCF_MoveStraight | $CFF03E | m1x0 | 73 | bankcf |
| BattleCF_MoveCircle | $CFF1F8 | m1x0 | 69 | bankcf |
| BattleCF_MoveKind4 | $CFF23D | m1x0 | 279 | bankcf |
| BattleCF_MoveEllipse | $CFF354 | m1x0 | 75 | bankcf |
| BattleCF_MoveShake | $CFF39F | m1x0 | 27 | bankcf |
| BattleCF_ShakeStep | $CFF3BA | m1x0 | 75 | bankcf |
| BattleCF_MoveCurve | $CFF405 | m1x0 | 173 | bankcf |
| BattleCF_ObjMoveStraight | $CFF4B2 | m1x0 | 81 | bankcf |
| BattleCF_ObjMoveArc | $CFF503 | m1x0 | 284 | bankcf |
| BattleCF_ObjMoveCircle | $CFF68D | m1x0 | 85 | bankcf |
| BattleCF_ObjMoveKind4 | $CFF6E2 | m1x0 | 282 | bankcf |
| BattleCF_ObjMoveOrbit | $CFF7FC | m1x0 | 136 | bankcf |
| BattleCF_ObjMoveCurve | $CFF884 | m1x0 | 211 | bankcf |
| BattleCF_PointsNear | $CFF957 | m1x0 | 33 | bankcf |
| BattleCF_StepEnemies | $CFF978 | m1x0 | 91 | bankcf |
| BattleFx_MulBySinValue | $CFFA17 | m1x0 | 44 | bankcf |
| Battle_CalcAngleCF | $CFFA43 | m1x0 | 154 | bankcf |
| BattleFD_UnkB655 | $FDB655 | m1x0 | 145 | bankfd |
| Sub_FDCA0E | $FDCA0E | m1x0 | 586 | bankfd |
| Loc_FDD07D | $FDD07D | m1x1 | 44 | bankfd |
| M7Scene_Run80 | $FDDA00 | m1x0 | 95 | bankfd |
| M7Scene_Run81 | $FDDA5F | m1x0 | 95 | bankfd |
| M7Scene_Run82 | $FDDABE | m1x0 | 95 | bankfd |
| M7Scene_Run83 | $FDDB1D | m1x0 | 122 | bankfd |
| M7Scene_Nmi | $FDDE15 | m1x0 | 114 | bankfd |
| M7Scene_Irq | $FDDE87 | m1x0 | 17 | bankfd |
| FieldFD_LoadAnimSetA | $FDDE98 | m1x0 | 394 | bankfd |
| FieldFD_UploadTileAnims | $FDE022 | m1x0 | 595 | bankfd |
| PalAnim_StopAll | $FDE275 | m1x1 | 29 | bankfd |
| FieldFD_LoadAnimSetB | $FDE292 | m1x0 | 266 | bankfd |
| PalAnim_TickAll | $FDE39C | m1x0 | 155 | bankfd |
| PalAnim_RotateUp | $FDE4E8 | m1x0 | 74 | bankfd |
| PalAnim_Pulse | $FDE57C | m1x0 | 107 | bankfd |
| PalAnim_PulseWhite | $FDE807 | m1x0 | 104 | bankfd |
| UNREACH_C0027E | $C0027E | m1x0 | 5 | bankc0 |
| CODE_C002AF | $C002AF | m1x0 | 280 | bankc0 |
| Field_ResumeCommonReload | $C00304 | m1x0 | 171 | bankc0 |
| Field_ExportPpuStateTo2C56 | $C0066D | m1x0 | 168 | bankc0 |
| CODE_C00CCE | $C00CCE | m1x0 | 89 | bankc0 |
| Event_ProcessPendingMenuRequest | $C00CF0 | m1x0 | 55 | bankc0 |
| Event_LaunchPendingMenuRequest | $C00D09 | m1x0 | 19 | bankc0 |
| Field_DispatchTileSwapTrigger | $C00D3E | m1x0 | 3003 | bankc0 |
| Field_ProcessTransitionRequest | $C01770 | m1x0 | 393 | bankc0 |
| Field_RedrawLayer3IfSelected | $C01862 | m1x0 | 42 | bankc0 |
| Field_LaunchBattle | $C0188C | m1x0 | 109 | bankc0 |
| UNREACH_C018CE | $C018CE | m1x0 | 43 | bankc0 |
| CODE_C0193E | $C0193E | m1x0 | 38 | bankc0 |
| CODE_C019A7 | $C019A7 | m1x0 | 43 | bankc0 |
| Party_ReadActiveSlots | $C01A5D | m1x0 | 79 | bankc0 |
| CODE_C01B22 | $C01B22 | m1x0 | 3 | bankc0 |
| UNREACH_C01B25 | $C01B25 | m1x0 | 7 | bankc0 |
| UNREACH_C01B32 | $C01B32 | m1x0 | 7 | bankc0 |
| AudioFadeDispatch | $C01BE6 | m1x0 | 278 | bankc0 |
| Field_RestoreMusic_PlaySong26 | $C01C90 | m1x0 | 108 | bankc0 |
| CODE_C01DA7 | $C01DA7 | m0x1 | 4 | bankc0 |
| Player_TryOpenTreasureChest | $C01DF4 | m1x0 | 180 | bankc0 |
| CODE_C01E09 | $C01E09 | m0x0, m1x0 | 3 | bankc0 |
| CODE_C01EDE | $C01EDE | m1x0 | 2 | bankc0 |
| Dialog_OpenSystemMessage | $C01F0E | m1x0 | 22 | bankc0 |
| CODE_C02081 | $C02081 | m1x0 | 5 | bankc0 |
| UNREACH_C0208B | $C0208B | m1x0 | 16 | bankc0 |
| UNREACH_C0212B | $C0212B | m1x0 | 3 | bankc0 |
| CODE_C021A3 | $C021A3 | m1x0 | 5 | bankc0 |
| UNREACH_C021C2 | $C021C2 | m1x0 | 31 | bankc0 |
| UNREACH_C0277E | $C0277E | m1x0 | 166 | bankc0 |
| CODE_C028D6 | $C028D6 | m0x0 | 3 | bankc0 |
| CODE_JL_C02918 | $C02918 | m1x0 | 4 | bankc0 |
| UNREACH_C0291C | $C0291C | m1x0 | 27 | bankc0 |
| UNREACH_C02933 | $C02933 | m1x0 | 41 | bankc0 |
| UNREACH_C02958 | $C02958 | m1x0 | 41 | bankc0 |
| CODE_JL_C029BA | $C029BA | m1x0 | 65 | bankc0 |
| CODE_JL_C02A32 | $C02A32 | m1x0 | 326 | bankc0 |
| UNREACH_C02C8C | $C02C8C | m1x0 | 316 | bankc0 |
| UNREACH_C02CDA | $C02CDA | m1x0 | 238 | bankc0 |
| CODE_JL_C02D0B | $C02D0B | m1x0 | 189 | bankc0 |
| CODE_JL_C02DC5 | $C02DC5 | m1x0 | 3 | bankc0 |
| ColorCrashEndsInfiniteLoop_2E65 | $C02E65 | m1x0 | 2 | bankc0 |
| UNREACH_C030A7 | $C030A7 | m1x0 | 12 | bankc0 |
| CODE_C031BB | $C031BB | m1x1 | 1 | bankc0 |
| Dialog_OpenDecisionBox | $C03674 | m1x0 | 54 | bankc0 |
| EventCommandCC_3845 | $C03845 | m1x0 | 22 | bankc0 |
| EventCommandCF_38A8 | $C038A8 | m1x0 | 22 | bankc0 |
| EventCommandD6_3A4C | $C03A4C | m1x0 | 28 | bankc0 |
| EventCommandFF_3FFC | $C03FFC | m1x0 | 4 | bankc0 |
| Obj_SetupFollower1Pc | $C04310 | m1x0 | 179 | bankc0 |
| EventCommands575C62_438A | $C0438A | m1x0 | 57 | bankc0 |
| Obj_SetupFollower2Pc | $C043C3 | m1x0 | 179 | bankc0 |
| EventCommands575C62_443D | $C0443D | m1x0 | 57 | bankc0 |
| EventCommand82PartialOverlap_464F | $C0464F | m1x0 | 18 | bankc0 |
| Obj_FinishNpcGfxSetup | $C04799 | m1x0 | 101 | bankc0 |
| Obj_DecompressNpcGfxToHeap | $C047FE | m1x0 | 245 | bankc0 |
| Obj_UseDirectGfxPtr | $C04845 | m1x0 | 135 | bankc0 |
| EventCommand88_48BD | $C048BD | m1x0 | 3 | bankc0 |
| EventCommand88_4911 | $C04911 | m0x0 | 8 | bankc0 |
| EventCmd_PaletteCmd_QueueObjTask | $C04970 | m1x0 | 136 | bankc0 |
| EventCmd_PaletteCmd_SetObjColors | $C049F8 | m1x0 | 86 | bankc0 |
| EventCommand2E_4A68 | $C04A68 | m1x0 | 82 | bankc0 |
| EventCmd_Op2E_QueuePaletteTask | $C04A72 | m1x0 | 114 | bankc0 |
| EventCommand7A_4DA1 | $C04DA1 | m1x0 | 132 | bankc0 |
| EventCmd_Follow_Core | $C05228 | m1x0 | 152 | bankc0 |
| EventCmd_MoveToward_Core | $C0536B | m1x0 | 165 | bankc0 |
| EventCommands0F171B_554B | $C0554B | m1x0 | 12 | bankc0 |
| EventCmd_FaceToward_Core | $C055A9 | m1x0 | 92 | bankc0 |
| EventCommandsA8A9_5600 | $C05600 | m1x0 | 5 | bankc0 |
| CODE_C05670 | $C05670 | m1x0 | 1 | bankc0 |
| EventScript_RunObjectScripts | $C058DE | m1x0 | 177 | bankc0 |
| EventScript_FetchNextCommand | $C05900 | m1x0 | 162 | bankc0 |
| CODE_C059F8 | $C059F8 | m1x1 | 90 | bankc0 |
| CODE_C05A25 | $C05A25 | m1x1 | 90 | bankc0 |
| CODE_C05C43 | $C05C43 | m0x1 | 4 | bankc0 |
| EventScript_InstallBuiltinScripts | $C05CEC | m1x0 | 75 | bankc0 |
| EventCmd_CallObjectFn_Core | $C05FBE | m1x0 | 192 | bankc0 |
| EventCmd_CallObjectFnSync_Core | $C06086 | m1x0 | 131 | bankc0 |
| EventCmd_CallObjectFnWait_Core | $C06122 | m1x0 | 139 | bankc0 |
| EventCmd_CallFnWait_Poll | $C061A7 | m1x0 | 66 | bankc0 |
| CODE_JL_C061AF | $C061AF | m1x0 | 58 | bankc0 |
| EventCmd_IfOp_SkipForward | $C06349 | m1x1 | 24 | bankc0 |
| EventCommand16OperatorRoutines_64B3 | $C064B3 | m1x1 | 2 | bankc0 |
| EventCommand16OperatorRoutines_651F | $C0651F | m0x1 | 4 | bankc0 |
| EventScript_SkipBlockByLen | $C066D0 | m1x0 | 21 | bankc0 |
| EventCommands2D3031_6701 | $C06701 | m1x0 | 4 | bankc0 |
| EventCommands2D3031_6784 | $C06784 | m1x0 | 4 | bankc0 |
| NewGameSave_CopyNameField | $C06897 | m0x0 | 14 | bankc0 |
| Field_LoadTilesetChunkTo6000 | $C06D65 | m1x0 | 106 | bankc0 |
| Obj_AssignFixedPaletteSlot | $C0719B | m1x1 | 119 | bankc0 |
| UNREACH_C071ED | $C071ED | m1x1 | 98 | bankc0 |
| CODE_JL_C0722A | $C0722A | m1x1 | 90 | bankc0 |
| UNREACH_C072B2 | $C072B2 | m1x1 | 2 | bankc0 |
| CODE_JL_C072C2 | $C072C2 | m1x1 | 90 | bankc0 |
| UNREACH_C0734A | $C0734A | m1x1 | 2 | bankc0 |
| Field_DrawL1TileRowStripOdd | $C07700 | m1x0 | 229 | bankc0 |
| CODE_JL_C077FE | $C077FE | m1x0 | 62 | bankc0 |
| CODE_JL_C07840 | $C07840 | m1x0 | 62 | bankc0 |
| Field_DrawL1TileColStripOdd | $C0787E | m1x0 | 233 | bankc0 |
| CODE_JL_C0792F | $C0792F | m0x0 | 31 | bankc0 |
| CODE_JL_C0794E | $C0794E | m1x0 | 129 | bankc0 |
| UNREACH_C079AE | $C079AE | m1x0 | 33 | bankc0 |
| Field_DrawL2TileRowStripOdd | $C07AC1 | m1x0 | 233 | bankc0 |
| CODE_JL_C07BC3 | $C07BC3 | m1x0 | 63 | bankc0 |
| CODE_JL_C07C06 | $C07C06 | m1x0 | 63 | bankc0 |
| Field_DrawL2TileColStripOdd | $C07C45 | m1x0 | 237 | bankc0 |
| UNREACH_C07D45 | $C07D45 | m1x0 | 33 | bankc0 |
| CODE_C07DE7 | $C07DE7 | m1x0 | 1 | bankc0 |
| Field_DrawL3TileRowStripOdd | $C07DE8 | m1x0 | 121 | bankc0 |
| CODE_JL_C07E7A | $C07E7A | m1x0 | 58 | bankc0 |
| CODE_JL_C07EB8 | $C07EB8 | m1x0 | 58 | bankc0 |
| Field_DrawL3TileColStripOdd | $C07EF2 | m1x0 | 217 | bankc0 |
| Camera_MoveRight | $C08AB5 | m1x0, m1x1 | 630 | bankc0 |
| Camera_MoveRight_Probe2 | $C08AE4 | m1x0 | 583 | bankc0 |
| CODE_C08B13 | $C08B13 | m1x0, m1x1 | 7 | bankc0 |
| Camera_MoveLeft | $C08BC4 | m1x0, m1x1 | 636 | bankc0 |
| Camera_MoveUp | $C08BF9 | m1x0, m1x1 | 639 | bankc0 |
| CODE_C08C57 | $C08C57 | m1x0, m1x1 | 7 | bankc0 |
| Camera_MoveDown | $C08D11 | m1x0, m1x1 | 631 | bankc0 |
| CODE_C08D67 | $C08D67 | m1x0, m1x1 | 7 | bankc0 |
| Camera_MoveUpRight | $C08E21 | m1x1 | 1118 | bankc0 |
| Camera_MoveUpLeft | $C08EF1 | m1x1 | 1124 | bankc0 |
| Camera_MoveDownLeft | $C08FC1 | m1x1 | 1127 | bankc0 |
| Camera_DownLeft_FallbackDown | $C0908A | m1x0, m1x1 | 640 | bankc0 |
| Camera_MoveDownRight | $C0909C | m1x1 | 1119 | bankc0 |
| Camera_DownRight_FallbackDown | $C09163 | m1x0, m1x1 | 640 | bankc0 |
| CODE_C091BE | $C091BE | m1x0 | 537 | bankc0 |
| CODE_C091DC | $C091DC | m1x0 | 495 | bankc0 |
| CODE_C0999D | $C0999D | m0x1 | 171 | bankc0 |
| LoadTileProperties_9AB2 | $C09AB2 | m1x0 | 22 | bankc0 |
| Camera_ComposeOutputs | $C09B41 | m1x0, m1x1 | 33 | bankc0 |
| Camera_ComposeOutputsAlt | $C09B62 | m1x0, m1x1 | 36 | bankc0 |
| Camera_ComposeOutputsByMode | $C09B86 | m1x0, m1x1 | 180 | bankc0 |
| CODE_C09BB7 | $C09BB7 | m1x0, m1x1 | 79 | bankc0 |
| CODE_C09BBE | $C09BBE | m1x0, m1x1 | 39 | bankc0 |
| CODE_C09BCF | $C09BCF | m1x0, m1x1 | 79 | bankc0 |
| CODE_C09C4F | $C09C4F | m1x0, m1x1 | 2 | bankc0 |
| Player_CheckExitTile | $C09D2C | m1x0, m1x1 | 151 | bankc0 |
| UNREACH_C09DA3 | $C09DA3 | m1x1 | 10 | bankc0 |
| Frame_PostScriptStage | $C09DC3 | m1x0 | 102 | bankc0 |
| Obj_SelectMoveAnimFromHistory | $C09ED1 | m1x1 | 102 | bankc0 |
| Obj_SelectMoveAnimFromHistory2 | $C09F20 | m1x1 | 102 | bankc0 |
| Obj_SelectMoveAnimFromHistoryVel | $C09F6F | m1x1 | 266 | bankc0 |
| UNREACH_C09FE7 | $C09FE7 | m1x1 | 11 | bankc0 |
| Obj_StartWalkAnim | $C0A171 | m1x1 | 27 | bankc0 |
| Obj_StartRunAnim | $C0A18C | m1x1 | 70 | bankc0 |
| Obj_SetIdleAnim | $C0A1E9 | m1x1 | 130 | bankc0 |
| CODE_C0A211 | $C0A211 | m0x1, m1x1 | 25 | bankc0 |
| Obj_StartAnim1B | $C0A254 | m1x1 | 23 | bankc0 |
| CODE_C0A641 | $C0A641 | m1x0 | 123 | bankc0 |
| CODE_C0A83E | $C0A83E | m1x1 | 116 | bankc0 |
| CODE_C0AA43 | $C0AA43 | m1x1 | 47 | bankc0 |
| CODE_C0AA6D | $C0AA6D | m1x1 | 4 | bankc0 |
| CODE_C0AA95 | $C0AA95 | m1x1 | 41 | bankc0 |
| CODE_C0AAB9 | $C0AAB9 | m1x1 | 4 | bankc0 |
| Obj_ApplyVelocityAirborne | $C0AAC7 | m1x1 | 54 | bankc0 |
| CODE_C0ADD7 | $C0ADD7 | m1x0 | 35 | bankc0 |
| CODE_C0ADEB | $C0ADEB | m1x0, m1x1 | 15 | bankc0 |
| Obj_MoveAtAngleSetPriority | $C0AE37 | m1x1 | 249 | bankc0 |
| UNREACH_C0AF04 | $C0AF04 | m1x0 | 35 | bankc0 |
| Obj_StoreMoveDeltas | $C0AF18 | m1x0, m1x1 | 15 | bankc0 |
| Obj_EmitOamEntriesSize1 | $C0B3DF | m1x0 | 217 | bankc0 |
| Obj_EmitOam8Entries_Loop | $C0B427 | m0x0 | 52 | bankc0 |
| CODE_JL_C0B45B | $C0B45B | m1x0 | 98 | bankc0 |
| CODE_JL_C0B489 | $C0B489 | m1x0 | 99 | bankc0 |
| Obj_EmitOamEntriesSize2 | $C0B4B8 | m1x0 | 247 | bankc0 |
| CODE_JL_C0B50A | $C0B50A | m0x0 | 52 | bankc0 |
| CODE_JL_C0B53E | $C0B53E | m1x0 | 108 | bankc0 |
| CODE_JL_C0B576 | $C0B576 | m1x0 | 109 | bankc0 |
| Obj_EmitOamEntriesSize3 | $C0B5AF | m1x0 | 338 | bankc0 |
| CODE_JL_C0B61F | $C0B61F | m0x0 | 52 | bankc0 |
| CODE_JL_C0B6AA | $C0B6AA | m1x0 | 139 | bankc0 |
| CODE_JL_C0BC50 | $C0BC50 | m1x0 | 140 | bankc0 |
| CODE_JL_C0BE46 | $C0BE46 | m1x0 | 214 | bankc0 |
| CODE_JL_C0BF1C | $C0BF1C | m1x0 | 214 | bankc0 |
| CODE_JL_C0C13B | $C0C13B | m1x0 | 206 | bankc0 |
| CODE_JL_C0C483 | $C0C483 | m1x0 | 318 | bankc0 |
| CODE_JL_C0C820 | $C0C820 | m1x0 | 362 | bankc0 |
| UNREACH_C0C875 | $C0C875 | m1x0 | 277 | bankc0 |
| CODE_C0C9C6 | $C0C9C6 | m1x1 | 119 | bankc0 |
| CODE_C0C9D0 | $C0C9D0 | m1x1 | 139 | bankc0 |
| Field_AnimComputeFramePtr | $C0CA18 | m1x1 | 94 | bankc0 |
| UNREACH_C0CB11 | $C0CB11 | m1x1 | 1 | bankc0 |
| Field_AnimReadFrameByte | $C0CBC2 | m1x0, m1x1 | 26 | bankc0 |
| Field_StreamFrameTilesToCache | $C0CC04 | m1x1 | 452 | bankc0 |
| UNREACH_C0D1A7 | $C0D1A7 | m1x0 | 227 | bankc0 |
| UNREACH_C0D1C1 | $C0D1C1 | m1x0 | 201 | bankc0 |
| UNREACH_C0D958 | $C0D958 | m1x0 | 273 | bankc0 |
| UNREACH_C0D9C0 | $C0D9C0 | m1x0 | 169 | bankc0 |
| UNREACH_C0DBAF | $C0DBAF | m1x0 | 377 | bankc0 |
| UNREACH_C0DD66 | $C0DD66 | m1x0 | 457 | bankc0 |
| UNREACH_C0DDCE | $C0DDCE | m1x0 | 353 | bankc0 |
| UNREACH_C0DDE8 | $C0DDE8 | m1x0 | 327 | bankc0 |
| UNREACH_C0DE02 | $C0DE02 | m1x0 | 301 | bankc0 |
| UNREACH_C0DE1C | $C0DE1C | m1x0 | 275 | bankc0 |
| UNREACH_C0DE6A | $C0DE6A | m1x0 | 197 | bankc0 |
| UNREACH_C0DE84 | $C0DE84 | m1x0 | 171 | bankc0 |
| CODE_JL_C0DF2F | $C0DF2F | m1x1 | 507 | bankc0 |
| UNREACH_C0DF61 | $C0DF61 | m1x0 | 457 | bankc0 |
| UNREACH_C0DF7B | $C0DF7B | m1x0 | 431 | bankc0 |
| UNREACH_C0DF95 | $C0DF95 | m1x0 | 405 | bankc0 |
| UNREACH_C0DFAF | $C0DFAF | m1x0 | 379 | bankc0 |
| UNREACH_C0DFE3 | $C0DFE3 | m1x0 | 327 | bankc0 |
| UNREACH_C0DFFD | $C0DFFD | m1x0 | 301 | bankc0 |
| UNREACH_C0E031 | $C0E031 | m1x0 | 249 | bankc0 |
| UNREACH_C0E065 | $C0E065 | m1x0 | 197 | bankc0 |
| UNREACH_C0E18D | $C0E18D | m0x0 | 961 | bankc0 |
| UNREACH_C0E212 | $C0E212 | m1x0 | 802 | bankc0 |
| UNREACH_C0E22C | $C0E22C | m1x0 | 776 | bankc0 |
| UNREACH_C0E260 | $C0E260 | m1x0 | 724 | bankc0 |
| UNREACH_C0E2AE | $C0E2AE | m1x0 | 646 | bankc0 |
| UNREACH_C0E2C8 | $C0E2C8 | m1x0 | 620 | bankc0 |
| UNREACH_C0E34A | $C0E34A | m1x0 | 490 | bankc0 |
| UNREACH_C0E364 | $C0E364 | m1x0 | 464 | bankc0 |
| UNREACH_C0E37E | $C0E37E | m1x0 | 438 | bankc0 |
| UNREACH_C0E398 | $C0E398 | m1x0 | 412 | bankc0 |
| UNREACH_C0E3E6 | $C0E3E6 | m1x0 | 334 | bankc0 |
| UNREACH_C0E400 | $C0E400 | m1x0 | 308 | bankc0 |
| UNREACH_C0E41A | $C0E41A | m1x0 | 282 | bankc0 |
| UNREACH_C0E434 | $C0E434 | m1x0 | 256 | bankc0 |
| CODE_C0EA51 | $C0EA51 | m1x1 | 19 | bankc0 |
| UNREACH_C0EACE | $C0EACE | m1x0 | 221 | bankc0 |
| UNREACH_C0EB46 | $C0EB46 | m1x0 | 282 | bankc0 |
| Vblank_DialogRowThenTail | $C0EB79 | m1x0 | 231 | bankc0 |
| Vblank_CommonTail | $C0EB86 | m1x0 | 218 | bankc0 |
| Vblank_CommonEnd | $C0EC15 | m1x0 | 75 | bankc0 |
| Vblank_AnimTileWords_Phase2 | $C0EE8B | m1x0 | 326 | bankc0 |
| Vblank_AnimTileWords_DoorCells | $C0EFD1 | m1x0 | 141 | bankc0 |
| CODE_C0F0C9 | $C0F0C9 | m1x0 | 7 | bankc0 |
| Mode7_VblankUpdate | $C308D0 | m0x0 | 117 | bankc3 |
| Mode7_ReadPads | $C30A8E | m1x0 | 113 | bankc3 |
| Sub_C20541 | $C20541 | m0x0 | 51 | bankc2 |
| Menu_DrawBlinkRowMarker | $C289FA | m0x0 | 55 | bankc2 |
| Menu_CursorBlinkGate | $C28B6E | m1x0 | 23 | bankc2 |
| Menu_DrawMainMenuOptionPanel | $C29B54 | m1x0 | 17 | bankc2 |
| Menu_HighlightMainMenuOption | $C29B8F | m1x0 | 79 | bankc2 |
| Menu_PlaceMainMenuCursorBar | $C29BDE | m0x0 | 75 | bankc2 |
| Menu_SetPanelAttrNibbleBoth | $C29EB0 | m0x0 | 29 | bankc2 |
| Menu_ItemCursorRowSelect | $C2AD2E | m1x0 | 34 | bankc2 |
| Sub_C2AD50 | $C2AD50 | m1x0 | 115 | bankc2 |
| Menu_DrawItemListRows | $C2AE43 | m1x0 | 34 | bankc2 |
| Menu_SetItemRowParams | $C2AE65 | m0x0 | 16 | bankc2 |
| Menu_SetItemRowParamsY | $C2AE75 | m1x0 | 124 | bankc2 |
| Menu_HighlightItemTabStrip | $C2B6AE | m1x0 | 37 | bankc2 |
| Menu_DrawTechCharStrip | $C2BA4F | m1x0 | 173 | bankc2 |
| Sub_C2C2FF | $C2C2FF | m1x0 | 152 | bankc2 |
| Menu_DrawConfigToggles | $C2C3E4 | m0x0 | 55 | bankc2 |
| Menu_ConfigCopySettings | $C2C43A | m0x0 | 28 | bankc2 |
| Menu_ConfigHighlightRows | $C2C456 | m0x0 | 63 | bankc2 |
| Menu_HighlightConfigChoice | $C2C47A | m0x0 | 27 | bankc2 |
| Menu_HighlightConfigRow3064 | $C2C495 | m0x0 | 39 | bankc2 |
| Menu_DrawButtonAssignGlyphs | $C2C57A | m0x0 | 45 | bankc2 |
| Menu_ButtonBitToGlyph | $C2C5B3 | m0x0 | 20 | bankc2 |
| PartyMenu_DimCharSpritePalette | $C2CCB3 | m0x0 | 40 | bankc2 |
| Menu_DrawSaveSlotHeaders | $C2D10D | m1x0 | 36 | bankc2 |
| Menu_WriteSaveSlotHeaderRow | $C2D131 | m1x0 | 101 | bankc2 |
| Menu_FillAttrBytesTwoRows | $C2ECAC | m0x0 | 22 | bankc2 |
| MenuOverlay_DrawWindowCurrentStyle | $C2ECDB | m0x0 | 45 | bankc2 |
| MenuOverlay_DrawWindowBorder | $C2EDF6 | m1x0, m0x0 | 45 | bankc2 |
| MenuOverlay_FillWindowBackground | $C2EE7F | m1x0, m0x0 | 118 | bankc2 |
| PointersToRo_C2F071 | $C2F071 | m0x0 | 31 | bankc2 |
| PointersToRo_C2F08D | $C2F08D | m0x0 | 21 | bankc2 |
| PointersToRo_C2F0A2 | $C2F0A2 | m0x0 | 22 | bankc2 |
| PointersToRo_C2F0B8 | $C2F0B8 | m0x0 | 9 | bankc2 |
| PointersToRo_C2F0D1 | $C2F0D1 | m0x0 | 67 | bankc2 |
| MenuSprite_AssignChars0to6 | $C2F566 | m1x0 | 34 | bankc2 |
| Menu_WriteTile16Row | $C2FBB4 | m1x0 | 26 | bankc2 |
| Menu_RestoreInventoryBackup | $C29121 | m1x0 | 22 | bankc2 |
| Menu_BuildMainMenuBgTilemap | $C29AAD | m1x0 | 167 | bankc2 |
| Menu_RedrawMainMenuOptionPanel | $C29B71 | m1x0 | 18 | bankc2 |
| Sub_C2ADC3 | $C2ADC3 | m1x0 | 13 | bankc2 |
| Sub_C2B8DA | $C2B8DA | m1x0 | 134 | bankc2 |
| Menu_DrawTechCharPanel | $C2BA2F | m0x0 | 32 | bankc2 |
| MenuSprite_PlaceCharRowSprite | $C2BAFC | m0x0 | 30 | bankc2 |
| Sub_C2CAE0 | $C2CAE0 | m1x0 | 6 | bankc2 |
| Sub_C2D065 | $C2D065 | m1x0 | 118 | bankc2 |
| Menu_RestoreCharTabRowBlock | $C2D36C | m1x0 | 243 | bankc2 |
| Menu_Draw2x2IconBlock | $C2D466 | m0x0 | 78 | bankc2 |
| TechType_Damage | $C1D2DE | m1x0 | 146 | bankc1 |
| Sub_C21813 | $C21813 | m0x0 | 6 | bankc2 |
| Sub_C30B95 | $C30B95 | m0x0 | 10 | bankc3 |
| Loc_C30CCD | $C30CCD | m0x0 | 21 | bankc3 |
| Sub_C30C51 | $C30C51 | m0x0 | 25 | bankc3 |
| Sub_C30C7C | $C30C7C | m0x0 | 9 | bankc3 |
| Sub_C30C85 | $C30C85 | m0x0 | 7 | bankc3 |
| Sub_C30C2A | $C30C2A | m0x0 | 32 | bankc3 |
| Sub_C30BD4 | $C30BD4 | m0x0 | 38 | bankc3 |
| Sub_C30BCC | $C30BCC | m0x0 | 19 | bankc3 |
| Sub_C30C5F | $C30C5F | m0x0 | 36 | bankc3 |
| Mode7ScriptOp_Jump | $C30B9F | m0x0 | 7 | bankc3 |
| Mode7Script_DoLoop | $C30C92 | m0x0 | 38 | bankc3 |
| Audio_ServiceExit | $C70192 | m1x0 | 15 | bankc7 |
| Audio_SendPlaySongCmd | $C70A39 | m1x0 | 95 | bankc7 |
| Loc_C2339A | $C2339A | m1x0 | 16 | bankc2 |
| Loc_C233AA | $C233AA | m1x0 | 42 | bankc2 |
| Loc_C233D4 | $C233D4 | m1x0 | 48 | bankc2 |
| Overworld_FetchTriggerScript | $C2626E | m1x0 | 24 | bankc2 |
| Loc_C26286 | $C26286 | m1x0 | 11 | bankc2 |
| Sub_CD07B9 | $CD07B9 | m1x0 | 41 | bankcd |
| BattleBg_ScrollHdma_PerLine | $CD07E2 | m1x0 | 171 | bankcd |
| Sub_CEE18E | $CEE18E | m1x0 | 57 | bankce |
| Sub_D1EA4B | $D1EA4B | m1x0 | 389 | bankd1 |
| Sub_D1EBD0 | $D1EBD0 | m1x0 | 15 | bankd1 |
| Sub_D1EBDF | $D1EBDF | m1x0 | 32 | bankd1 |

## Implemented opcodes

$00 BRK imm, $01 ORA dp_x_ind, $02 COP imm, $03 ORA sr, $04 TSB dp, $05 ORA dp, $06 ASL dp, $07 ORA dp_ind_long, $08 PHP impl, $09 ORA imm_m, $0A ASL A, $0B PHD impl, $0C TSB abs, $0D ORA abs, $0E ASL abs, $0F ORA long, $10 BPL rel, $11 ORA dp_ind_y, $12 ORA dp_ind, $13 ORA sr_ind_y, $14 TRB dp, $15 ORA dp_x, $16 ASL dp_x, $17 ORA dp_ind_long_y, $18 CLC impl, $19 ORA abs_y, $1A INC A, $1B TCS impl, $1C TRB abs, $1D ORA abs_x, $1E ASL abs_x, $1F ORA long_x, $20 JSR abs, $21 AND dp_x_ind, $22 JSL long, $23 AND sr, $24 BIT dp, $25 AND dp, $26 ROL dp, $27 AND dp_ind_long, $28 PLP impl, $29 AND imm_m, $2A ROL A, $2B PLD impl, $2C BIT abs, $2D AND abs, $2E ROL abs, $2F AND long, $30 BMI rel, $31 AND dp_ind_y, $32 AND dp_ind, $33 AND sr_ind_y, $34 BIT dp_x, $35 AND dp_x, $36 ROL dp_x, $37 AND dp_ind_long_y, $38 SEC impl, $39 AND abs_y, $3A DEC A, $3B TSC impl, $3C BIT abs_x, $3D AND abs_x, $3E ROL abs_x, $3F AND long_x, $40 RTI impl, $41 EOR dp_x_ind, $42 WDM imm, $43 EOR sr, $44 MVP block, $45 EOR dp, $46 LSR dp, $47 EOR dp_ind_long, $48 PHA impl, $49 EOR imm_m, $4A LSR A, $4B PHK impl, $4C JMP abs, $4D EOR abs, $4E LSR abs, $4F EOR long, $50 BVC rel, $51 EOR dp_ind_y, $52 EOR dp_ind, $53 EOR sr_ind_y, $54 MVN block, $55 EOR dp_x, $56 LSR dp_x, $57 EOR dp_ind_long_y, $58 CLI impl, $59 EOR abs_y, $5A PHY impl, $5B TCD impl, $5C JML long, $5D EOR abs_x, $5E LSR abs_x, $5F EOR long_x, $60 RTS impl, $61 ADC dp_x_ind, $62 PER rlong, $63 ADC sr, $64 STZ dp, $65 ADC dp, $66 ROR dp, $67 ADC dp_ind_long, $68 PLA impl, $69 ADC imm_m, $6A ROR A, $6B RTL impl, $6C JMP abs_ind, $6D ADC abs, $6E ROR abs, $6F ADC long, $70 BVS rel, $71 ADC dp_ind_y, $72 ADC dp_ind, $73 ADC sr_ind_y, $74 STZ dp_x, $75 ADC dp_x, $76 ROR dp_x, $77 ADC dp_ind_long_y, $78 SEI impl, $79 ADC abs_y, $7A PLY impl, $7B TDC impl, $7C JMP abs_x_ind, $7D ADC abs_x, $7E ROR abs_x, $7F ADC long_x, $80 BRA rel, $81 STA dp_x_ind, $82 BRL rlong, $83 STA sr, $84 STY dp, $85 STA dp, $86 STX dp, $87 STA dp_ind_long, $88 DEY impl, $89 BIT imm_m, $8A TXA impl, $8B PHB impl, $8C STY abs, $8D STA abs, $8E STX abs, $8F STA long, $90 BCC rel, $91 STA dp_ind_y, $92 STA dp_ind, $93 STA sr_ind_y, $94 STY dp_x, $95 STA dp_x, $96 STX dp_y, $97 STA dp_ind_long_y, $98 TYA impl, $99 STA abs_y, $9A TXS impl, $9B TXY impl, $9C STZ abs, $9D STA abs_x, $9E STZ abs_x, $9F STA long_x, $A0 LDY imm_x, $A1 LDA dp_x_ind, $A2 LDX imm_x, $A3 LDA sr, $A4 LDY dp, $A5 LDA dp, $A6 LDX dp, $A7 LDA dp_ind_long, $A8 TAY impl, $A9 LDA imm_m, $AA TAX impl, $AB PLB impl, $AC LDY abs, $AD LDA abs, $AE LDX abs, $AF LDA long, $B0 BCS rel, $B1 LDA dp_ind_y, $B2 LDA dp_ind, $B3 LDA sr_ind_y, $B4 LDY dp_x, $B5 LDA dp_x, $B6 LDX dp_y, $B7 LDA dp_ind_long_y, $B8 CLV impl, $B9 LDA abs_y, $BA TSX impl, $BB TYX impl, $BC LDY abs_x, $BD LDA abs_x, $BE LDX abs_y, $BF LDA long_x, $C0 CPY imm_x, $C1 CMP dp_x_ind, $C2 REP imm, $C3 CMP sr, $C4 CPY dp, $C5 CMP dp, $C6 DEC dp, $C7 CMP dp_ind_long, $C8 INY impl, $C9 CMP imm_m, $CA DEX impl, $CB WAI impl, $CC CPY abs, $CD CMP abs, $CE DEC abs, $CF CMP long, $D0 BNE rel, $D1 CMP dp_ind_y, $D2 CMP dp_ind, $D3 CMP sr_ind_y, $D4 PEI dp, $D5 CMP dp_x, $D6 DEC dp_x, $D7 CMP dp_ind_long_y, $D8 CLD impl, $D9 CMP abs_y, $DA PHX impl, $DB STP impl, $DC JML abs_ind_long, $DD CMP abs_x, $DE DEC abs_x, $DF CMP long_x, $E0 CPX imm_x, $E1 SBC dp_x_ind, $E2 SEP imm, $E3 SBC sr, $E4 CPX dp, $E5 SBC dp, $E6 INC dp, $E7 SBC dp_ind_long, $E8 INX impl, $E9 SBC imm_m, $EA NOP impl, $EB XBA impl, $EC CPX abs, $ED SBC abs, $EE INC abs, $EF SBC long, $F0 BEQ rel, $F1 SBC dp_ind_y, $F2 SBC dp_ind, $F3 SBC sr_ind_y, $F4 PEA abs, $F5 SBC dp_x, $F6 INC dp_x, $F7 SBC dp_ind_long_y, $F8 SED impl, $F9 SBC abs_y, $FA PLX impl, $FB XCE impl, $FC JSR abs_x_ind, $FD SBC abs_x, $FE INC abs_x, $FF SBC long_x

## Tests

| Test | Result |
|---|---|
| runtime | Passed |
| runtime_fatal_open_bus | Passed |
| runtime_fatal_rom_write | Passed |
| runtime_fatal_unhooked | Passed |
| runtime_fatal_entry | Passed |
| snes_bus | Passed |
| hw_map | Passed |
| interp_modes | Passed |
| interp_system | Passed |
| sched | Passed |
| stack_guard | Passed |
| replay | Passed |
| resample | Passed |
| dsp | Passed |
| native | Passed |
| lockstep_report | Passed |
| ref_compare_report | Passed |
| ctest_cache_report | Passed |
| tas_convert | Passed |
| decode_calls | Passed |
| funcs_meta | Passed |
| check_agnostic | Passed |
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
| c0_spine_vectors | Passed |
| interp_c0_spine_vectors | Passed |
| diff_all_0 | Passed |
| diff_all_1 | Passed |
| diff_all_2 | Passed |
| diff_all_3 | Passed |
| diff_all_4 | Passed |
| diff_all_5 | Passed |
| diff_all_6 | Passed |
| diff_all_7 | Passed |
| diff_open_bus | Passed |
| sync_determinism | Skipped |
| manual_root | Passed |
| discover_loop | Passed |
| discover_table | Passed |
| decode_boundary | Passed |
| decode_plp | Passed |
| find_overlays | Passed |
| label_encoding | Passed |
| decompress | Passed |
| boot_interp | Passed |
| boot_menu | Passed |
| lockstep_boot | Passed |
| lockstep_leene_square | Passed |
| lockstep_gato_battle | Passed |
| lockstep_gato_win | Passed |
| lockstep_telepod_gate | Passed |
| lockstep_marle_meeting | Passed |
| lockstep_marle_joins | Passed |
| lockstep_telepod_exhibit | Passed |
| lockstep_telepod_ride | Passed |
| lockstep_canyon_guardia | Passed |
| lockstep_loc118_entry | Passed |
| lockstep_fair_wander | Passed |
| stack_field_replay | Passed |
| battle_no_flicker | Passed |
| native_coverage | Passed |
