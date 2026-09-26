cmake_minimum_required(VERSION 3.20)
project(ct_recomp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

find_package(Python3 3.11 REQUIRED COMPONENTS Interpreter)

if(NOT DEFINED ENV{CT_ROM})
    message(FATAL_ERROR "CT_ROM is not set (path to headerless US 1.0 ROM)")
endif()
set(CT_ROM $ENV{CT_ROM})
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/check_rom.py ${CT_ROM}
    RESULT_VARIABLE rom_ok)
if(NOT rom_ok EQUAL 0)
    message(FATAL_ERROR "CT_ROM check failed: ${CT_ROM}")
endif()

set(CT_WARN -Wall -Wextra -Wpedantic)

enable_testing()

# ---- vendored SNES core (third_party/snes/, see its README.md) ----
add_library(ct_snes_vendor STATIC
    third_party/snes/ppu.c third_party/snes/dma.c
    third_party/snes/apu.c third_party/snes/spc.c third_party/snes/dsp.c)
target_include_directories(ct_snes_vendor PUBLIC third_party/snes third_party)
# Not our code; don't hold it to our warning flags.

# ---- runtime ----
add_library(ct_runtime STATIC runtime/cpu.c runtime/bus.c runtime/hooks.c
    runtime/snes_adapter.c runtime/png.c)
target_include_directories(ct_runtime PUBLIC runtime)
target_link_libraries(ct_runtime PUBLIC ct_snes_vendor)
target_compile_options(ct_runtime PRIVATE ${CT_WARN})

# ---- generated code ----
set(CT_OUT ${CMAKE_SOURCE_DIR}/out)
file(GLOB CT_RECOMP_PY ${CMAKE_SOURCE_DIR}/recomp/*.py)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/funcs.toml)
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/recomp/emit.py --list-modules
    OUTPUT_VARIABLE CT_MODULES OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE ct_list_ok)
if(NOT ct_list_ok EQUAL 0)
    message(FATAL_ERROR "emit.py --list-modules failed")
endif()
set(CT_GEN_OUTPUTS ${CT_OUT}/ct_funcs.h ${CT_OUT}/ct_funcs.c)
set(CT_GEN_SOURCES ${CT_OUT}/ct_funcs.c)
foreach(m ${CT_MODULES})
    list(APPEND CT_GEN_OUTPUTS ${CT_OUT}/${m}.c ${CT_OUT}/${m}.h)
    list(APPEND CT_GEN_SOURCES ${CT_OUT}/${m}.c)
endforeach()
add_custom_command(
    OUTPUT ${CT_GEN_OUTPUTS}
    COMMAND ${CMAKE_COMMAND} -E env CT_ROM=${CT_ROM}
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/recomp/emit.py --out ${CT_OUT}
    DEPENDS ${CT_RECOMP_PY} ${CMAKE_SOURCE_DIR}/funcs.toml
    COMMENT "emit ${CT_MODULES}"
    VERBATIM)
add_library(ct_gen STATIC ${CT_GEN_SOURCES})
target_include_directories(ct_gen PUBLIC ${CT_OUT})
target_link_libraries(ct_gen PUBLIC ct_runtime)
target_compile_options(ct_gen PRIVATE ${CT_WARN} -Wno-unused-label)
# ct_gen has no non-test consumer yet (pre-Title Screen); revisit when one exists.
target_compile_definitions(ct_gen PRIVATE CT_TEST_BUILD)

# ---- interpreter: strict test oracle, system mode from reset (interp.h) ----
# Reads ct_gen's extern and jump-table tables, so it sits on top of ct_gen.
add_library(ct_interp STATIC runtime/interp.c)
target_link_libraries(ct_interp PUBLIC ct_gen)
target_compile_options(ct_interp PRIVATE ${CT_WARN})

# ---- frame scheduler and boot probe (sched.h) ----
add_library(ct_sched STATIC runtime/sched.c)
target_link_libraries(ct_sched PUBLIC ct_interp)
target_compile_options(ct_sched PRIVATE ${CT_WARN})

add_executable(ct_boot tools/ct_boot.c)
target_link_libraries(ct_boot ct_sched)
target_compile_options(ct_boot PRIVATE ${CT_WARN})

# ---- tests ----

add_executable(test_runtime test/test_runtime.c)
target_link_libraries(test_runtime ct_gen ct_interp)
target_compile_options(test_runtime PRIVATE ${CT_WARN})
add_test(NAME runtime COMMAND test_runtime)
foreach(f open_bus rom_write unhooked decimal entry)
    add_test(NAME runtime_fatal_${f} COMMAND test_runtime fatal_${f})
endforeach()
set_tests_properties(runtime_fatal_open_bus PROPERTIES PASS_REGULAR_EXPRESSION "ct: read8 \\$006000: open bus")
set_tests_properties(runtime_fatal_rom_write PROPERTIES PASS_REGULAR_EXPRESSION "ct: write8 \\$C10000: write to ROM")
set_tests_properties(runtime_fatal_unhooked PROPERTIES PASS_REGULAR_EXPRESSION "ct: read8 \\$004200: unhooked")
set_tests_properties(runtime_fatal_decimal PROPERTIES PASS_REGULAR_EXPRESSION "ct: \\$C10000: ADC with D=1")
set_tests_properties(runtime_fatal_entry PROPERTIES PASS_REGULAR_EXPRESSION "ct: \\$C10089: entry state m0 x0 e0, expected m1 x0")

add_executable(test_c1_math test/test_c1_math.c)
target_link_libraries(test_c1_math ct_gen ct_interp)
target_compile_options(test_c1_math PRIVATE ${CT_WARN})
foreach(t mul8 mulaccum divide shifts)
    add_test(NAME c1_math_${t} COMMAND test_c1_math ${t})
    add_test(NAME interp_c1_math_${t} COMMAND test_c1_math ${t} interp)
endforeach()

add_executable(test_c1_text test/test_c1_text.c)
target_link_libraries(test_c1_text ct_gen ct_interp)
target_compile_options(test_c1_text PRIVATE ${CT_WARN})
foreach(t format divten reencode)
    add_test(NAME c1_text_${t} COMMAND test_c1_text ${t})
    add_test(NAME interp_c1_text_${t} COMMAND test_c1_text ${t} interp)
endforeach()

add_executable(test_c1_ui test/test_c1_ui.c)
target_link_libraries(test_c1_ui ct_gen ct_interp)
target_compile_options(test_c1_ui PRIVATE ${CT_WARN})
add_test(NAME c1_ui_gauge COMMAND test_c1_ui)
add_test(NAME interp_c1_ui_gauge COMMAND test_c1_ui interp)

add_executable(test_snes_bus test/test_snes_bus.c)
target_link_libraries(test_snes_bus ct_gen ct_interp)
target_compile_options(test_snes_bus PRIVATE ${CT_WARN})
add_test(NAME snes_bus COMMAND test_snes_bus)

add_executable(test_interp_modes test/test_interp_modes.c)
target_link_libraries(test_interp_modes ct_gen ct_interp)
target_compile_options(test_interp_modes PRIVATE ${CT_WARN})
add_test(NAME interp_modes COMMAND test_interp_modes)

add_executable(test_interp_system test/test_interp_system.c)
target_link_libraries(test_interp_system ct_gen ct_interp)
target_compile_options(test_interp_system PRIVATE ${CT_WARN})
add_test(NAME interp_system COMMAND test_interp_system)

add_executable(test_sched test/test_sched.c)
target_link_libraries(test_sched ct_sched)
target_compile_options(test_sched PRIVATE ${CT_WARN})
add_test(NAME sched COMMAND test_sched)

add_executable(test_stack_guard test/test_stack_guard.c)
target_link_libraries(test_stack_guard ct_gen ct_interp)
target_compile_options(test_stack_guard PRIVATE ${CT_WARN})
add_test(NAME stack_guard COMMAND test_stack_guard)

add_executable(test_diff test/test_diff.c)
target_link_libraries(test_diff ct_gen ct_interp)
target_compile_options(test_diff PRIVATE ${CT_WARN})
# Full 2000-trial sweep, split into shards so ctest -j runs them in parallel.
set(CT_DIFF_SHARDS 8)
math(EXPR ct_last_shard "${CT_DIFF_SHARDS} - 1")
foreach(k RANGE ${ct_last_shard})
    add_test(NAME diff_all_${k} COMMAND test_diff 2000 --shard ${k}/${CT_DIFF_SHARDS})
endforeach()

add_test(NAME sync_determinism COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/test_sync.py)
set_tests_properties(sync_determinism PROPERTIES SKIP_RETURN_CODE 77)

add_test(NAME manual_root COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/test_manual_root.py)

# ---- test environment (keep last) ----
get_property(ct_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${ct_tests} PROPERTIES ENVIRONMENT "CT_ROM=${CT_ROM}")
