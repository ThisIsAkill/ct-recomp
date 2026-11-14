# Chrono Trigger (USA) 1.0: symbols (funcs.toml), extern hooks, generated
# code, the frontends linked against it, and every ROM-dependent test.

set(GAME_DIR ${CMAKE_CURRENT_SOURCE_DIR})
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/check_rom.py ${GAME_DIR} ${CT_ROM}
    RESULT_VARIABLE rom_ok)
if(NOT rom_ok EQUAL 0)
    message(FATAL_ERROR "CT_ROM check failed for ${GAME_DIR}: ${CT_ROM}")
endif()

# ---- generated code (into the build tree) ----
set(CT_OUT ${CMAKE_CURRENT_BINARY_DIR}/out)
set(CT_FUNCS_TOML ${GAME_DIR}/funcs.toml)
file(GLOB CT_RECOMP_PY ${CMAKE_SOURCE_DIR}/recomp/*.py)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CT_FUNCS_TOML})
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/recomp/emit.py --list-modules
            --funcs ${CT_FUNCS_TOML}
    OUTPUT_VARIABLE CT_MODULES OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE ct_list_ok)
if(NOT ct_list_ok EQUAL 0)
    message(FATAL_ERROR "emit.py --list-modules failed")
endif()
# Code the game decompresses into WRAM (#92): images from the ROM, checked
# against overlays.toml's hashes, then recompiled with the rest.
set(CT_OVERLAYS_TOML ${GAME_DIR}/overlays.toml)
set(CT_OVERLAY_MANIFEST ${CT_OUT}/overlays/manifest.toml)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CT_OVERLAYS_TOML})
add_custom_command(
    OUTPUT ${CT_OVERLAY_MANIFEST}
    COMMAND ${CMAKE_COMMAND} -E env CT_ROM=${CT_ROM}
            ${Python3_EXECUTABLE} ${GAME_DIR}/tools/overlay_images.py ${CT_OVERLAYS_TOML}
            ${CT_OUT}/overlays
    DEPENDS ${CT_OVERLAYS_TOML} ${GAME_DIR}/tools/overlay_images.py
            ${GAME_DIR}/tools/ct_decompress.py
    COMMENT "overlay images"
    VERBATIM)

set(CT_GEN_OUTPUTS ${CT_OUT}/ct_funcs.h ${CT_OUT}/ct_funcs.c ${CT_OUT}/ct_overlays.c)
set(CT_GEN_SOURCES ${CT_OUT}/ct_overlays.c)
foreach(m ${CT_MODULES})
    list(APPEND CT_GEN_OUTPUTS ${CT_OUT}/${m}.c ${CT_OUT}/${m}.h)
    list(APPEND CT_GEN_SOURCES ${CT_OUT}/${m}.c)
endforeach()
add_custom_command(
    OUTPUT ${CT_GEN_OUTPUTS}
    COMMAND ${CMAKE_COMMAND} -E env CT_ROM=${CT_ROM}
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/recomp/emit.py --out ${CT_OUT}
            --funcs ${CT_FUNCS_TOML} --overlays ${CT_OVERLAY_MANIFEST}
    DEPENDS ${CT_RECOMP_PY} ${CT_FUNCS_TOML} ${CT_OVERLAY_MANIFEST}
    COMMENT "emit ${CT_MODULES}"
    VERBATIM)

# Recompiled functions plus this game's extern hooks.
add_library(ct_gen STATIC ${CT_GEN_SOURCES} hooks.c)
target_include_directories(ct_gen PUBLIC ${CT_OUT})
target_link_libraries(ct_gen PUBLIC ct_runtime ct_sched)
target_compile_options(ct_gen PRIVATE ${CT_WARN} -Wno-unused-label)
target_compile_definitions(ct_gen PRIVATE CT_TEST_BUILD)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(ct_gen PRIVATE ${CT_GEN_OPT})   # see CT_GEN_OPT
endif()

# The tables (func_table.h), as objects so they precede ct_interp.
add_library(ct_gen_tables OBJECT ${CT_OUT}/ct_funcs.c)
target_include_directories(ct_gen_tables PUBLIC ${CT_OUT})
target_link_libraries(ct_gen_tables PUBLIC ct_runtime)
target_compile_options(ct_gen_tables PRIVATE ${CT_WARN})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(ct_gen_tables PRIVATE ${CT_GEN_OPT})
endif()

function(ct_game_exe name)
    add_executable(${name} ${ARGN})
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/test)
    target_link_libraries(${name} ct_sched ct_gen_tables ct_gen)
    target_compile_options(${name} PRIVATE ${CT_WARN})
endfunction()

# ---- frontends, at the top of the build tree ----
ct_game_exe(ct_boot ${CMAKE_SOURCE_DIR}/tools/ct_boot.c)
set_target_properties(ct_boot PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
if(SDL2_FOUND)
    ct_game_exe(ct_sdl ${CMAKE_SOURCE_DIR}/frontend/main.c)
    target_link_libraries(ct_sdl ct_input)
    set_target_properties(ct_sdl PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# ---- tests (all ROM-dependent) ----
set(ct_game_tests)

ct_game_exe(test_c1_math test/test_c1_math.c)
foreach(t mul8 mulaccum divide shifts)
    add_test(NAME c1_math_${t} COMMAND test_c1_math ${t})
    add_test(NAME interp_c1_math_${t} COMMAND test_c1_math ${t} interp)
    list(APPEND ct_game_tests c1_math_${t} interp_c1_math_${t})
endforeach()

ct_game_exe(test_c1_text test/test_c1_text.c)
foreach(t format divten reencode)
    add_test(NAME c1_text_${t} COMMAND test_c1_text ${t})
    add_test(NAME interp_c1_text_${t} COMMAND test_c1_text ${t} interp)
    list(APPEND ct_game_tests c1_text_${t} interp_c1_text_${t})
endforeach()

ct_game_exe(test_c1_ui test/test_c1_ui.c)
add_test(NAME c1_ui_gauge COMMAND test_c1_ui)
add_test(NAME interp_c1_ui_gauge COMMAND test_c1_ui interp)
list(APPEND ct_game_tests c1_ui_gauge interp_c1_ui_gauge)

# The reset path's NMI/IRQ trampolines ($C00B64, $C00B75), native and interpreted.
ct_game_exe(test_c0_spine test/test_c0_spine.c)
add_test(NAME c0_spine_vectors COMMAND test_c0_spine)
add_test(NAME interp_c0_spine_vectors COMMAND test_c0_spine interp)
list(APPEND ct_game_tests c0_spine_vectors interp_c0_spine_vectors)

# Every recompiled function vs the interpreter, 2000 trials, in shards.
ct_game_exe(test_diff ${CMAKE_SOURCE_DIR}/test/test_diff.c)
set(CT_DIFF_SHARDS 8)
math(EXPR ct_last_shard "${CT_DIFF_SHARDS} - 1")
foreach(k RANGE ${ct_last_shard})
    add_test(NAME diff_all_${k} COMMAND test_diff 2000 --shard ${k}/${CT_DIFF_SHARDS})
    list(APPEND ct_game_tests diff_all_${k})
endforeach()

# Regression (#29): Sub_B309 trial 32 reads the open-bus $2188 right after
# LDA long,X's operand fetch; generated code must leave the operand's last
# byte on the bus as the interpreter does.
add_test(NAME diff_open_bus COMMAND test_diff 200 Sub_B309)
list(APPEND ct_game_tests diff_open_bus)

add_test(NAME sync_determinism COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_sync.py)
set_tests_properties(sync_determinism PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME manual_root COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_manual_root.py)
add_test(NAME discover_loop COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_discover_loop.py)
add_test(NAME discover_table COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_discover_table.py)
add_test(NAME decode_boundary COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_decode_boundary.py)
add_test(NAME decode_plp COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_decode_plp.py)
add_test(NAME find_overlays COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_find_overlays.py)
add_test(NAME label_encoding COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_label_encoding.py)
# ct_boot --sram-in/--sram-out on a synthetic ROM the test writes itself
# (#109): no game ROM needed, so no rom label (it sets its own CT_ROM).
add_test(NAME sram_boot COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/test_sram_boot.py
    $<TARGET_FILE:ct_boot>)
list(APPEND ct_game_tests sync_determinism manual_root)

# tools/ct_decompress.py against the game's own decompressor, call by call.
add_test(NAME decompress COMMAND ${Python3_EXECUTABLE} ${GAME_DIR}/test/test_decompress.py
    ${CMAKE_BINARY_DIR})
list(APPEND ct_game_tests decompress)

# Boot from reset in the interpreter to frame 1500, on the title screen:
# no fatal, NMIs running (1387 by then; the bound leaves room for timing
# fixes), frame not black,
# audio produced.
add_test(NAME boot_interp COMMAND ct_boot --frames 1500 --min-nmis 1350 --require-render
    --require-audio)
# Start on the title (frames 1500-1505): a new game opens the Battle Mode
# menu through Menu_InitPpu ($C2940D), which writes the read-only
# $4216-$4219. Found in ct_sdl, where the keyboard drives pad 1.
add_test(NAME boot_menu COMMAND ct_boot --frames 1700 --input 1500-1505:start
    --expect-pc C2940D --require-render)
list(APPEND ct_game_tests boot_interp boot_menu)

# Native dispatch vs the interpreter alone: per-frame state hashes taken at
# the frame edge must match on every frame (tools/lockstep.py reports the
# first divergent one). The boot to the title; a new game through
# Crono's house and the world map into Leene Square up to the bell; and on
# from there into Gato's fight, the first battle ($C10089 Battle_Mul8 runs
# only in battle).
add_test(NAME lockstep_boot COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/lockstep.py
    $<TARGET_FILE:ct_boot> --frames 1500)
# Idle on the title into the attract demos: battle code there returns by
# rewriting the stacked return address (RTS as a jump), which compiled
# callers must hand off, not fatal on (frame 6779 before the hand-off);
# and the $7E3000 dispatcher's patched JSR operand (#97, overlays.toml)
# must stay native under strict overlay checks.
add_test(NAME lockstep_attract COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/lockstep.py
    $<TARGET_FILE:ct_boot> --frames 16000)
add_test(NAME lockstep_leene_square COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 9210
    --script ${GAME_DIR}/test/replay/leene_square.txt --expect-pc C2940D)
add_test(NAME lockstep_gato_battle COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 11000
    --script ${GAME_DIR}/test/replay/gato_battle.txt --expect-pc C10089)
# The first battle won: by frame 13400 Gato's HP (battle record 6,
# $7E6133) is 0 and Crono's (BatWork_CurHp, $7E5E30) is not.
add_test(NAME lockstep_gato_win COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 13950
    --script ${GAME_DIR}/test/replay/gato_win.txt
    --expect-wram 13400:6133=0 --expect-wram 13400:6134=0 --expect-wram 13400:5E30!=0)
# To the Telepod gate at the top of the bell screen; the attendant's
# "still setting up" dialogue (exhibit not ready on this route yet).
add_test(NAME lockstep_telepod_gate COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 21500
    --script ${GAME_DIR}/test/replay/telepod_gate.txt)
# The Marle meeting at Leene's Bell: bump into her, the pendant dialogue,
# and Marle following - story event code past the first battle.
add_test(NAME lockstep_marle_meeting COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 11800
    --script ${GAME_DIR}/test/replay/marle_meeting.txt)
# Marle formally joins: bump, talk, pendant pickup at tile (30, 8),
# return it, and the name-entry screen; party slot 2 ($7E2981) holds
# Marle's character id 1 at the end.
add_test(NAME lockstep_marle_joins COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 24000
    --script ${GAME_DIR}/test/replay/marle_joins.txt
    --expect-wram 24000:2981=1)
# Through the Telepod exhibit: the shopkeeper talk sets the gate flag,
# Lucca's talk advances the story 8 -> 10, and Marle's talk runs the
# Telepod demonstration; she vanishes and the story flag ($7F0000)
# is 12 at the end.
add_test(NAME lockstep_telepod_exhibit COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 96600
    --script ${GAME_DIR}/test/replay/telepod_exhibit.txt
    --expect-wram 96600:10000=C)
# Crono's own ride: step onto the left Telepod pad; Lucca's machine
# sends him through the Gate to the 600 A.D. canyon (location 113),
# where he is object 2 at tile (8.5, 8.9) at the end.
add_test(NAME lockstep_telepod_ride COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 105000
    --script ${GAME_DIR}/test/replay/telepod_ride.txt
    --expect-wram 105000:1805=8)
# Through Truce Canyon (location 112) and out the south exit onto the
# 600 A.D. overworld (location 497, WRAM $7E0100 = F1).
add_test(NAME lockstep_canyon_guardia COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 130000
    --script ${GAME_DIR}/test/replay/canyon_guardia.txt
    --expect-wram 130000:100=F1)
# On the 600 A.D. overworld, tile (45, 36) facing north with an A press
# enters location 118 at spawn (56, 11), story flag still $0C
# (WRAM $7E0100 = 76 at the end).
add_test(NAME lockstep_loc118_entry COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 136500
    --script ${GAME_DIR}/test/replay/loc118_entry.txt
    --expect-wram 136500:100=76)
# Into Truce (location 119) in 600 A.D. via the BFS-guided overworld
# bridge route; enters at frame 179168 (WRAM $7E0100 = 77 at the end),
# story flag still $0C.
add_test(NAME lockstep_truce_entry COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 183800
    --script ${GAME_DIR}/test/replay/truce_entry.txt
    --expect-wram 183800:100=77)
# Through Truce's north exit to the overworld, north to Guardia Castle
# (location 120), and the throne-room audience: the story flag
# ($7F0000) advances $0C -> $0F at frame 197750 (WRAM $7E0100 = 78).
add_test(NAME lockstep_castle_audience COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 201500
    --script ${GAME_DIR}/test/replay/castle_audience.txt
    --expect-wram 201500:10000=F)
# After the win: scripted wandering around Leene Square fair (direction
# holds and A presses), exercising field movement, collision and NPC code
# past the first battle.
add_test(NAME lockstep_fair_wander COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/lockstep.py $<TARGET_FILE:ct_boot> --frames 19350
    --script ${GAME_DIR}/test/replay/fair_wander.txt)
# The C stack stays flat through a long stretch of field play (#101): the
# field loop there jumps between compiled functions every frame, which used
# to nest a C frame each time (275 KB by frame 10600; now about 2 KB).
add_test(NAME stack_field_replay COMMAND $<TARGET_FILE:ct_boot> --frames 10600
    --script ${GAME_DIR}/test/replay/gato_win.txt --overlay-strict --max-stack 32768)
list(APPEND ct_game_tests lockstep_boot lockstep_attract lockstep_leene_square lockstep_gato_battle
    lockstep_gato_win lockstep_fair_wander lockstep_marle_meeting lockstep_telepod_gate stack_field_replay
    lockstep_marle_joins lockstep_telepod_exhibit lockstep_telepod_ride lockstep_canyon_guardia
    lockstep_loc118_entry lockstep_truce_entry lockstep_castle_audience)

# Battle sprites must not drop out on alternate frames (Gato's fight from
# its start; broken until OAMADD kept its own reload value, test_hw_map).
add_test(NAME battle_no_flicker COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/test_flicker.py
    $<TARGET_FILE:ct_boot> 10680 11100 --script ${GAME_DIR}/test/replay/gato_battle.txt)
list(APPEND ct_game_tests battle_no_flicker)

# Native coverage must not regress: share of instructions run natively
# through the first battle (50.6% when added; 49.2% once HDMA took its
# CPU time, which shortens native wait loops; ct_boot --profile N lists
# the functions the rest ran in).
add_test(NAME native_coverage COMMAND ct_boot --frames 11000 --min-native 49
    --script ${GAME_DIR}/test/replay/gato_battle.txt)
list(APPEND ct_game_tests native_coverage)

if(SDL2_FOUND)
    # The frontend itself, headless (SDL dummy drivers), unpaced.
    add_test(NAME sdl_boot COMMAND ct_sdl --frames 300 --fast --require-render)
    # --record while playing the Leene Square replay; the recording must
    # reproduce the run frame for frame in ct_boot.
    add_test(NAME sdl_record COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/test_record.py
        $<TARGET_FILE:ct_sdl> $<TARGET_FILE:ct_boot> ${GAME_DIR}/test/replay/leene_square.txt 9210)
    # 60 s of boot in real time through the SDL audio queue (dummy driver,
    # 960-sample buffer = 20 ms, which it consumes at the true rate): no
    # underrun once playback starts.
    add_test(NAME sdl_audio_60s COMMAND ct_sdl --frames 3606 --require-no-underrun)
    # Real-time pacing: other tests running beside it starve the audio queue.
    set_tests_properties(sdl_audio_60s PROPERTIES RUN_SERIAL TRUE)
    list(APPEND ct_game_tests sdl_boot sdl_record sdl_audio_60s)
endif()

set_tests_properties(${ct_game_tests} PROPERTIES LABELS rom ENVIRONMENT
    "CT_ROM=${CT_ROM};DSCOTTON_DIR=$ENV{DSCOTTON_DIR};SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
