# Progress

Written by `tools/progress.py`. Do not edit by hand.

| Metric | Value |
|---|---|
| Routines recompiled | 395 |
| Emitted C functions (routine x entry state) | 409 |
| ROM bytes covered | 38991 |
| Functions known total (validated + unresolved) | 1033 |
| Functions validated | 395 |
| Functions unresolved (pending sync) | 638 |
| Opcodes implemented | 251 / 256 |
| Opcodes used by recompiled routines | 138 / 256 |
| Opcode x width combinations implemented | 446 |
| Tests passing | 24 / 24 |
| Test assertions checked | 610246364 |

## Coverage by bank

| Bank | Bytes |
|---|---|
| $C0 | 25564 |
| $C1 | 9062 |
| $C2 | 2095 |
| $C3 | 863 |
| $CD | 887 |
| $CF | 302 |
| $D1 | 218 |

## Symbol sync by bank

| Bank | Validated | Unresolved | Known total |
|---|---|---|---|
| $C0 | 190 | 638 | 828 |
| $C1 | 106 | 0 | 106 |
| $C2 | 71 | 0 | 71 |
| $C3 | 1 | 0 | 1 |
| $CD | 21 | 0 | 21 |
| $CF | 5 | 0 | 5 |
| $D1 | 1 | 0 | 1 |

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
| InitHW | $C00B4E | m1x0 | 22 | bankc0 |
| Field_InstallNmiVector | $C00B64 | m1x0 | 17 | bankc0 |
| Field_InstallIrqVector | $C00B75 | m1x0 | 17 | bankc0 |
| Field_InitLocationStateVars | $C00B86 | m1x0 | 240 | bankc0 |
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
| Field_UploadWindowFrameGfx | $C06E5C | m1x0 | 68 | bankc0 |
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
| Obj_FindAtPosition | $C09923 | m1x1 | 187 | bankc0 |
| Camera_ClampVelToMapEdges | $C099DE | m1x0 | 65 | bankc0 |
| Camera_CheckBottomEdgeLimit | $C09A1F | m1x0 | 30 | bankc0 |
| Camera_CheckTopEdgeLimit | $C09A3D | m1x0 | 35 | bankc0 |
| Camera_CheckRightEdgeLimit | $C09A60 | m1x0 | 30 | bankc0 |
| Camera_CheckLeftEdgeLimit | $C09A7E | m1x0 | 35 | bankc0 |
| Camera_CheckZoneTable | $C09AA1 | m1x1 | 39 | bankc0 |
| Player_TilePropsLookup | $C09AC8 | m1x0, m1x1 | 33 | bankc0 |
| Camera_UpdateScroll | $C09AD3 | m1x0, m1x1 | 290 | bankc0 |
| Camera_CheckZoneMatch | $C09C37 | m1x1 | 37 | bankc0 |
| Camera_ApplyVelocity | $C09C5C | m0x1 | 359 | bankc0 |
| CODE_FN_C0A508 | $C0A508 | m1x0 | 1 | bankc0 |
| Field_WriteScreenDesignation | $C0A509 | m1x0 | 24 | bankc0 |
| Map_BuildExitGrid | $C0A66B | m1x0 | 167 | bankc0 |
| Map_BuildTreasureGrid | $C0A712 | m1x0 | 170 | bankc0 |
| Map_IsTreasureOpened | $C0A7BC | m1x0 | 45 | bankc0 |
| Field_ClearShadowOam | $C0A7E9 | m1x0 | 39 | bankc0 |
| Obj_RefreshVisibilityWindow | $C0A947 | m1x1 | 67 | bankc0 |
| Obj_WriteOamEntry | $C0A98A | m1x1 | 67 | bankc0 |
| Obj_YSortListInsert | $C0A9CD | m1x1 | 58 | bankc0 |
| Obj_ApplyMoveVelocity | $C0AA07 | m1x1 | 246 | bankc0 |
| Obj_ApplyMoveVelocityLinear | $C0AAFD | m1x1 | 126 | bankc0 |
| Obj_ComputeScreenPos | $C0AB45 | m1x1 | 93 | bankc0 |
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
| Sub_CB3A | $C0CB3A | m1x0, m1x1 | 162 | bankc0 |
| Sub_E12A | $C0E12A | m1x0 | 1034 | bankc0 |
| Sub_E534 | $C0E534 | m0x0 | 339 | bankc0 |
| Sub_E687 | $C0E687 | m0x0 | 686 | bankc0 |
| Sub_E935 | $C0E935 | m1x0 | 29 | bankc0 |
| Sub_E952 | $C0E952 | m1x0, m1x1 | 33/40 | bankc0 |
| Sub_E9AA | $C0E9AA | m1x0, m1x1 | 85/56 | bankc0 |
| Sub_E9E2 | $C0E9E2 | m1x0 | 29 | bankc0 |
| Sub_E9FF | $C0E9FF | m1x0 | 32 | bankc0 |
| Sub_EA1F | $C0EA1F | m1x0 | 35 | bankc0 |
| Obj_ReleaseVramCells | $C0EA42 | m1x1 | 33 | bankc0 |
| Sub_EC60 | $C0EC60 | m1x0 | 23 | bankc0 |
| Map_BuildTilePropGrid | $C0A521 | m1x0 | 330 | bankc0 |

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
| sync_determinism | Passed |
