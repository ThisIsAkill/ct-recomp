cmake_minimum_required(VERSION 3.20)
project(ct_recomp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

find_package(Python3 3.11 REQUIRED COMPONENTS Interpreter)

set(CT_WARN -Wall -Wextra -Wpedantic)

enable_testing()

# The translator (recomp/), runtime, frontend, and engine tests are
# game-agnostic; each game under game/ supplies its symbols, generated
# code, and ROM-dependent tests. check_agnostic enforces the split.

# ---- vendored SNES core (third_party/snes/, see its README.md) ----
add_library(ct_snes_vendor STATIC
    third_party/snes/ppu.c third_party/snes/dma.c
    third_party/snes/apu.c third_party/snes/spc.c third_party/snes/dsp.c)
target_include_directories(ct_snes_vendor PUBLIC third_party/snes third_party)
# Not our code; don't hold it to our warning flags.

# ---- runtime ----
add_library(ct_runtime STATIC runtime/cpu.c runtime/bus.c
    runtime/snes_adapter.c runtime/png.c runtime/wav.c runtime/hwlog.c
    runtime/cycles.c)
target_include_directories(ct_runtime PUBLIC runtime)
target_link_libraries(ct_runtime PUBLIC ct_snes_vendor)
target_compile_options(ct_runtime PRIVATE ${CT_WARN})

# Function tables (func_table.h): a game's generated ct_funcs.c, or these
# empty ones. OBJECT libraries, so the tables are always on the link line
# ahead of the interpreter that reads them.
add_library(ct_tables_empty OBJECT runtime/func_table_empty.c)
target_link_libraries(ct_tables_empty PUBLIC ct_runtime)
target_compile_options(ct_tables_empty PRIVATE ${CT_WARN})

# ---- interpreter: strict test oracle, system mode from reset (interp.h) ----
add_library(ct_interp STATIC runtime/interp.c)
target_link_libraries(ct_interp PUBLIC ct_runtime)
target_compile_options(ct_interp PRIVATE ${CT_WARN})

# ---- frame scheduler (sched.h) ----
add_library(ct_sched STATIC runtime/sched.c)
target_link_libraries(ct_sched PUBLIC ct_interp)
target_compile_options(ct_sched PRIVATE ${CT_WARN})

# ---- SDL2 input mapping (optional: skipped when SDL2 isn't installed) ----
find_package(SDL2 CONFIG QUIET)
if(SDL2_FOUND)
    add_library(ct_input STATIC frontend/input.c)
    target_include_directories(ct_input PUBLIC frontend)
    target_link_libraries(ct_input PUBLIC SDL2::SDL2)
    target_compile_options(ct_input PRIVATE ${CT_WARN})
else()
    message(STATUS "SDL2 not found: frontends not built")
endif()

# ---- engine tests: no game, no ROM (synthetic image, test/harness.h) ----
set(CT_TEST_ROM ${CMAKE_BINARY_DIR}/test_rom.sfc)
add_custom_command(
    OUTPUT ${CT_TEST_ROM}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/make_test_rom.py ${CT_TEST_ROM}
    DEPENDS ${CMAKE_SOURCE_DIR}/tools/make_test_rom.py
    COMMENT "synthetic test ROM"
    VERBATIM)
add_custom_target(ct_test_rom DEPENDS ${CT_TEST_ROM})

function(ct_engine_test name src)
    add_executable(${name} ${src})
    target_include_directories(${name} PRIVATE test)
    target_link_libraries(${name} ct_sched ct_tables_empty)
    target_compile_options(${name} PRIVATE ${CT_WARN})
    target_compile_definitions(${name} PRIVATE CT_TEST_ROM="${CT_TEST_ROM}")
    add_dependencies(${name} ct_test_rom)
endfunction()

ct_engine_test(test_runtime test/test_runtime.c)
add_test(NAME runtime COMMAND test_runtime)
foreach(f open_bus rom_write unhooked decimal entry)
    add_test(NAME runtime_fatal_${f} COMMAND test_runtime fatal_${f})
endforeach()
set_tests_properties(runtime_fatal_open_bus PROPERTIES PASS_REGULAR_EXPRESSION "ct: read8 \\$006000: open bus")
set_tests_properties(runtime_fatal_rom_write PROPERTIES PASS_REGULAR_EXPRESSION "ct: write8 \\$C10000: write to ROM")
set_tests_properties(runtime_fatal_unhooked PROPERTIES PASS_REGULAR_EXPRESSION "ct: read8 \\$004016: unhooked")
set_tests_properties(runtime_fatal_decimal PROPERTIES PASS_REGULAR_EXPRESSION "ct: \\$C10000: ADC with D=1")
set_tests_properties(runtime_fatal_entry PROPERTIES PASS_REGULAR_EXPRESSION "ct: \\$C08100: entry state m0 x0 e0, expected m1 x0")

foreach(t snes_bus hw_map interp_modes interp_system sched stack_guard)
    ct_engine_test(test_${t} test/test_${t}.c)
    add_test(NAME ${t} COMMAND test_${t})
endforeach()

if(SDL2_FOUND)
    add_executable(test_input test/test_input.c)
    target_link_libraries(test_input ct_input)
    target_compile_options(test_input PRIVATE ${CT_WARN})
    add_test(NAME input COMMAND test_input)
endif()

# No game-specific names, addresses, or words outside game/.
add_test(NAME check_agnostic
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/check_agnostic.py ${CMAKE_SOURCE_DIR})

# ---- games (need $CT_ROM; their tests are LABELS rom) ----
if(DEFINED ENV{CT_ROM})
    set(CT_ROM $ENV{CT_ROM})
    add_subdirectory(game/ct)
else()
    message(STATUS "CT_ROM not set: game builds and ROM tests skipped")
endif()

# ---- test environment (keep last) ----
# SDL dummy drivers: the frontend test runs headless.
get_property(ct_engine_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${ct_engine_tests} PROPERTIES ENVIRONMENT
    "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
