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
