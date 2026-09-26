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
set(CT_GEN_OUTPUTS ${CT_OUT}/ct_funcs.h ${CT_OUT}/ct_funcs.c)
set(CT_GEN_SOURCES)
foreach(m ${CT_MODULES})
    list(APPEND CT_GEN_OUTPUTS ${CT_OUT}/${m}.c ${CT_OUT}/${m}.h)
    list(APPEND CT_GEN_SOURCES ${CT_OUT}/${m}.c)
endforeach()
add_custom_command(
    OUTPUT ${CT_GEN_OUTPUTS}
    COMMAND ${CMAKE_COMMAND} -E env CT_ROM=${CT_ROM}
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/recomp/emit.py --out ${CT_OUT}
            --funcs ${CT_FUNCS_TOML}
    DEPENDS ${CT_RECOMP_PY} ${CT_FUNCS_TOML}
    COMMENT "emit ${CT_MODULES}"
    VERBATIM)

# Recompiled functions plus this game's extern hooks.
add_library(ct_gen STATIC ${CT_GEN_SOURCES} hooks.c)
target_include_directories(ct_gen PUBLIC ${CT_OUT})
target_link_libraries(ct_gen PUBLIC ct_runtime)
target_compile_options(ct_gen PRIVATE ${CT_WARN} -Wno-unused-label)
target_compile_definitions(ct_gen PRIVATE CT_TEST_BUILD)

# The tables (func_table.h), as objects so they precede ct_interp.
add_library(ct_gen_tables OBJECT ${CT_OUT}/ct_funcs.c)
target_include_directories(ct_gen_tables PUBLIC ${CT_OUT})
target_link_libraries(ct_gen_tables PUBLIC ct_runtime)
target_compile_options(ct_gen_tables PRIVATE ${CT_WARN})

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
list(APPEND ct_game_tests sync_determinism manual_root)

# Boot from reset in the interpreter to frame 1500, on the title screen:
# no fatal, NMIs running (1394 by then, deterministic), frame not black,
# audio produced.
add_test(NAME boot_interp COMMAND ct_boot --frames 1500 --min-nmis 1394 --require-render
    --require-audio)
# Start on the title (frames 1500-1505): a new game opens the Battle Mode
# menu through Menu_InitPpu ($C2940D), which writes the read-only
# $4216-$4219. Found in ct_sdl, where the keyboard drives pad 1.
add_test(NAME boot_menu COMMAND ct_boot --frames 1700 --input 1500-1505:start
    --expect-pc C2940D --require-render)
list(APPEND ct_game_tests boot_interp boot_menu)

if(SDL2_FOUND)
    # The frontend itself, headless (SDL dummy drivers), unpaced.
    add_test(NAME sdl_boot COMMAND ct_sdl --frames 300 --fast --require-render)
    list(APPEND ct_game_tests sdl_boot)
endif()

set_tests_properties(${ct_game_tests} PROPERTIES LABELS rom ENVIRONMENT
    "CT_ROM=${CT_ROM};DSCOTTON_DIR=$ENV{DSCOTTON_DIR};SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
