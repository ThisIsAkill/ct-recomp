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

# ---- tests ----

add_library(ct_interp STATIC test/interp.c)
target_include_directories(ct_interp PUBLIC test)
target_link_libraries(ct_interp PUBLIC ct_runtime)
target_compile_options(ct_interp PRIVATE ${CT_WARN})

add_executable(test_runtime test/test_runtime.c)
target_link_libraries(test_runtime ct_gen ct_interp)
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

add_executable(test_diff test/test_diff.c)
target_link_libraries(test_diff ct_gen ct_interp)
target_compile_options(test_diff PRIVATE ${CT_WARN})
add_test(NAME diff_all COMMAND test_diff 2000)

# ---- test environment (keep last) ----
get_property(ct_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${ct_tests} PROPERTIES ENVIRONMENT "CT_ROM=${CT_ROM}")
