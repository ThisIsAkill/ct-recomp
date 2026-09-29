# Progress

Written by `tools/progress.py`. Do not edit by hand.

| Metric | Value |
|---|---|
| Routines recompiled | 1341 |
| Emitted C functions (routine x entry state) | 1408 |
| ROM bytes covered | 126783 |
| Functions known total (validated + pending + unresolved) | 1963 |
| Functions validated | 1341 |
| Functions unresolved (pending sync) | 622 |
| Manual roots not yet emittable | 0 |
| Opcodes implemented | 251 / 256 |
| Opcodes used by recompiled routines | 190 / 256 |
| Opcode x width combinations implemented | 446 |
| Tests passing | 65 / 65 |
| Test assertions checked | 616231156 |

## Coverage by bank

| Bank | Bytes |
|---|---|
| $00 | 8 |
| $C0 | 58297 |
| $C1 | 23745 |
| $C2 | 19108 |
| $C3 | 1417 |
| $C7 | 1401 |
| $CC | 3561 |
| $CD | 2969 |
| $CE | 291 |
| $CF | 1646 |
| $D1 | 1272 |
| $FD | 12470 |
| $FF | 598 |

## Symbol sync by bank

| Bank | Validated | Unresolved | Known total |
|---|---|---|---|
| $00 | 2 | 0 | 2 |
| $C0 | 577 | 622 | 1199 |
| $C1 | 214 | 0 | 214 |
| $C2 | 350 | 0 | 350 |
| $C3 | 11 | 0 | 11 |
| $C7 | 8 | 0 | 8 |
| $CC | 4 | 0 | 4 |
| $CD | 53 | 0 | 53 |
| $CE | 2 | 0 | 2 |
| $CF | 13 | 0 | 13 |
| $D1 | 9 | 0 | 9 |
| $FD | 92 | 0 | 92 |
| $FF | 6 | 0 | 6 |

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
| Sub_0905 | $C00905 | m1x0 | 19 | bankc0 |
| Sub_0918 | $C00918 | m1x0 | 19 | bankc0 |
| Field_CalcLocationRecordOffset | $C0092B | m1x0 | 53 | bankc0 |
| Field_LoadTilesetGfx | $C00960 | m1x0 | 231 | bankc0 |
| Field_LoadTileAssembly12 | $C009DD | m1x0 | 55 | bankc0 |
| Field_LoadTileAssemblyL3 | $C00A14 | m1x0 | 60 | bankc0 |
| Field_RebuildMapVideoState | $C00A50 | m1x0 | 175 | bankc0 |
| Field_EnableNmiIrqAfterInit | $C00B28 | m1x0 | 38 | bankc0 |
| InitHW | $C00B4E | m1x0 | 22 | bankc0 |
| Field_InstallNmiVector | $C00B64 | m1x0 | 17 | bankc0 |
| Field_InstallIrqVector | $C00B75 | m1x0 | 17 | bankc0 |
| Field_InitLocationStateVars | $C00B86 | m1x0 | 240 | bankc0 |
| Sub_18D9 | $C018D9 | m1x0 | 139 | bankc0 |
| Sub_1985 | $C01985 | m1x0 | 77 | bankc0 |
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
| NewGameSave_DecompressToWork | $C056D4 | m1x0 | 128 | bankc0 |
| Field_ClearSpriteUploadQueue | $C05929 | m1x0 | 51 | bankc0 |
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
| Field_HideReservedOamSprites | $C0B262 | m1x0 | 15 | bankc0 |
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
| ReentryVectors | $C00000 | m1x0 | 173 | bankc0 |
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
| Menu_ShellEntryVector | $C20000 | m1x0 | 106 | bankc2 |
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
| Party_DispatchVector | $C28004 | m1x0 | 6 | bankc2 |
| Menu_LoadPartyD1Recs | $C2834D | m0x0, m1x1, m1x0 | 56 | bankc2 |
| Item_AddToInventory | $C28791 | m1x0 | 68 | bankc2 |
| Item_RemoveFromInventory | $C287D5 | m1x0 | 45 | bankc2 |
| Item_FindInventorySlot | $C287FA | m1x1 | 28 | bankc2 |
| Item_ClassifyId | $C28881 | m0x0, m1x0 | 38 | bankc2 |
| Party_DispatchOp | $C28C36 | m1x0 | 37 | bankc2 |
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
| Audio_Init_Entry | $C70000 | m1x0 | 282 | bankc7 |
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
| Sub_FDFFE5 | $FDFFE5 | m1x0 | 125 | bankfd |
| Warp_TransitionShortStub | $FDFFE8 | m1x0 | 98 | bankfd |
| Warp_GateEntryStub | $FDFFEB | m1x0 | 98 | bankfd |
| Warp_TransitionStub | $FDFFF1 | m1x0 | 98 | bankfd |
| PalAnim_LoadMapConfigStub | $FDFFF4 | m1x0 | 269 | bankfd |
| AnimTile_FrameTickStub | $FDFFF7 | m1x0 | 158 | bankfd |
| FdCore_ResourceServiceStub | $FDFFFA | m1x0 | 397 | bankfd |
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
| FdCore_VramDmaServiceStub | $FDFFFD | m1x0 | 598 | bankfd |
| Menu_CheckRockTriples | $FFF958 | m1x0, m0x0, m1x1 | 99 | bankff |
| SaveSlot_Validate | $FFF9C4 | m1x1, m1x0 | 55 | bankff |
| SaveSlot_GetBase | $FFFBD3 | m0x0 | 14 | bankff |
| SaveSlot_Checksum | $FFFBE7 | m0x0 | 29 | bankff |
| Sub_00FF10 | $00FF10 | m0x0, m1x0, m1x1 | 4 | bank00 |
| Sub_00FF14 | $00FF14 | m1x0, m0x0 | 4 | bank00 |
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
| Sub_C06FB9 | $C06FB9 | m1x1 | 1 | bankc0 |
| Sub_C06FBA | $C06FBA | m1x1 | 156 | bankc0 |
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
| Menu_LoadCharRecordToBufKeepX | $C28816 | m0x0 | 10 | bankc2 |
| Menu_LoadCharRecordToBuf | $C28839 | m1x0 | 72 | bankc2 |
| Item_GetStatsPtr | $C288AD | m1x0 | 43 | bankc2 |
| Item_GetStatsPtr2 | $C288D8 | m1x0 | 40 | bankc2 |
| Menu_CommitComputedStats | $C29137 | m0x0 | 55 | bankc2 |
| Menu_ComputeEquippedStats | $C2916E | m0x0 | 109 | bankc2 |
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
| PartyMenu_LoadStatsPanelGfx | $C2968D | m0x0 | 500 | bankc2 |
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
| NameEntry_BuildGlyphGrid | $C2E545 | m1x0 | 49 | bankc2 |
| NameEntry_LoadKeyboardGfx | $C2E576 | m1x0 | 93 | bankc2 |
| Menu_DrawSlotDescFFCF3B | $C2E5D3 | m1x0 | 56 | bankc2 |
| Menu_ResetOamPlaceCursor | $C2E92D | m0x0 | 79 | bankc2 |
| Menu_UpdateSelectionInput | $C2E984 | m1x0 | 85 | bankc2 |
| Menu_CursorMoveFromPad | $C2E9E2 | m1x0 | 61 | bankc2 |
| Menu_CursorNodeIndex | $C2EA27 | m1x0, m1x1 | 15 | bankc2 |
| Menu_PlayCursorSfx | $C2EAC2 | m1x0 | 10 | bankc2 |
| Menu_PlayConfirmSfx | $C2EACC | m1x0 | 10 | bankc2 |
| Menu_PlaySfx | $C2EB03 | m1x1 | 28 | bankc2 |
| Menu_Draw6x6GlyphTileBlock | $C2EC38 | m1x0 | 91 | bankc2 |
| MenuOverlay_LoadWindowStylePalette | $C2ED08 | m0x0 | 41 | bankc2 |
| Menu_RunFFUiCommandList | $C2ED31 | m1x0, m0x0 | 40 | bankc2 |
| Sub_C2ED77 | $C2ED77 | m1x0 | 25 | bankc2 |
| Menu_TilemapAddrFromRowCol | $C2ED90 | m0x0, m1x0 | 26 | bankc2 |
| Sub_C2EDAA | $C2EDAA | m1x0 | 124 | bankc2 |
| MenuOverlay_DrawWindowFrameCmd | $C2EDB0 | m1x0 | 51 | bankc2 |
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
| MenuSprite_GetCharD1RecAddr | $C2F5ED | m1x0 | 57 | bankc2 |
| MenuSprite_TickAnim | $C2F69D | m1x1 | 164 | bankc2 |
| Sub_C2F6D1 | $C2F6D1 | m0x0 | 1 | bankc2 |
| MenuSprite_QueueFrameVramDma | $C2F75C | m0x0 | 277 | bankc2 |
| Menu_RenderTextBlocking | $C2F90C | m0x0, m1x0 | 55 | bankc2 |
| Menu_StartDescTextRender | $C2FA49 | m1x0 | 86 | bankc2 |
| Menu_UploadRenderedText | $C2FAE9 | m1x0 | 174 | bankc2 |
| Menu_WriteTile16RowAttr31 | $C2FB97 | m1x0 | 29 | bankc2 |
| Sub_C30AFF | $C30AFF | m0x0 | 4 | bankc3 |
| Mode7Script_Dispatch | $C30B03 | m0x0 | 157 | bankc3 |
| Audio_Process_Entry | $C70004 | m1x0, m1x1 | 641 | bankc7 |
| Audio_UploadBlock2F00 | $C70922 | m1x0 | 184 | bankc7 |
| Battle_VramUploadFontTiles | $CCE22E | m1x0 | 98 | bankcc |
| Battle_InitDisplayState | $CCE2E4 | m1x0 | 2723 | bankcc |
| Battle_BuildEnemyOamTemplate | $CCED87 | m1x0 | 732 | bankcc |
| Battle_CallReencodeTextBuffer | $CCF063 | m1x0 | 8 | bankcc |
| BattleSys_BootVec | $CD0000 | m1x0 | 8 | bankcd |
| BattleSys_EnablePauseInputVec | $CD0003 | m1x0 | 8 | bankcd |
| BattleSys_PlaySong08StartFade | $CD000C | m1x0 | 28 | bankcd |
| BattleSys_PollTransitionVec | $CD0012 | m1x0 | 11 | bankcd |
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
| BattleSys_DrawUnitBackAnimObjects | $CFEDB8 | m1x0 | 254 | bankcf |
| BattleSys_DrawUnitFrontAnimObjects | $CFEDF7 | m1x0 | 53 | bankcf |
| BattleSys_TickMoverSlots | $CFEFAF | m1x0 | 202 | bankcf |
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
| input | Passed |
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
| sync_determinism | Passed |
| manual_root | Passed |
| decompress | Passed |
| boot_interp | Passed |
| boot_menu | Passed |
| lockstep_boot | Passed |
| lockstep_leene_square | Passed |
| lockstep_gato_battle | Passed |
| lockstep_gato_win | Passed |
| battle_no_flicker | Passed |
| native_coverage | Passed |
| sdl_boot | Passed |
| sdl_record | Passed |
| sdl_audio_60s | Passed |
