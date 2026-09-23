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

# ---- runtime ----
add_library(ct_runtime STATIC runtime/cpu.c runtime/bus.c)
target_include_directories(ct_runtime PUBLIC runtime)
target_compile_options(ct_runtime PRIVATE ${CT_WARN})

# ---- tests ----
add_executable(test_runtime test/test_runtime.c)
target_link_libraries(test_runtime ct_runtime)
target_compile_options(test_runtime PRIVATE ${CT_WARN})
add_test(NAME runtime COMMAND test_runtime)
foreach(f open_bus rom_write unhooked decimal entry)
    add_test(NAME runtime_fatal_${f} COMMAND test_runtime fatal_${f})
endforeach()
set_tests_properties(runtime_fatal_open_bus PROPERTIES PASS_REGULAR_EXPRESSION "ct: read8 \\$006000: open bus")
set_tests_properties(runtime_fatal_rom_write PROPERTIES PASS_REGULAR_EXPRESSION "ct: write8 \\$C10000: write to ROM")
set_tests_properties(runtime_fatal_unhooked PROPERTIES PASS_REGULAR_EXPRESSION "ct: read8 \\$002100: unhooked")
set_tests_properties(runtime_fatal_decimal PROPERTIES PASS_REGULAR_EXPRESSION "ct: \\$C10000: ADC with D=1")
set_tests_properties(runtime_fatal_entry PROPERTIES PASS_REGULAR_EXPRESSION "ct: \\$C10089: entry state m0 x0 e0, expected m1 x0")

# ---- test environment (keep last) ----
get_property(ct_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${ct_tests} PROPERTIES ENVIRONMENT "CT_ROM=${CT_ROM}")
