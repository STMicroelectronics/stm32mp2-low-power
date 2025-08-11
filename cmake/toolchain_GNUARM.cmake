#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
message(STATUS "stm32mp2-lp toolchain...")

set(CMAKE_SYSTEM_NAME			Generic)
set(CMAKE_SYSTEM_PROCESSOR		arm)

find_program(CMAKE_C_COMPILER	${CROSS_COMPILE}-gcc HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CXX_COMPILER ${CROSS_COMPILE}-g++ HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJCOPY	${CROSS_COMPILE}-objcopy HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJDUMP	${CROSS_COMPILE}-objdump HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_SIZE		${CROSS_COMPILE}-size HINTS ${TOOLCHAIN_BIN_PATH})

if(CMAKE_C_COMPILER STREQUAL "CMAKE_C_COMPILER-NOTFOUND")
    message(FATAL_ERROR "Could not find compiler: '${CROSS_COMPILE}gcc'")
endif()

if(CMAKE_CXX_COMPILER STREQUAL "CMAKE_CXX_COMPILER-NOTFOUND")
    message(FATAL_ERROR "Could not find compiler: '${CROSS_COMPILE}g++'")
endif()

if(CMAKE_OBJCOPY STREQUAL "CMAKE_OBJCOPY-NOTFOUND")
    message(FATAL_ERROR "Could not find objcopy: '${CROSS_COMPILE}objcopy'")
endif()

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

set(CMAKE_SYSTEM_PROCESSOR	"cortex-m33+nofp")
set(CMAKE_C_FLAGS_INIT		"-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_CXX_FLAGS_INIT	"-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_ASM_FLAGS_INIT	"-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_C_LINK_FLAGS		"-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_ASM_LINK_FLAGS	"-mcpu=${CMAKE_SYSTEM_PROCESSOR}")

add_compile_options(
    -specs=nano.specs
    -specs=nosys.specs
    -Wall
    -Wno-format
    -Wno-return-type
    -Wno-unused-but-set-variable
    -c
    -fdata-sections
    -ffunction-sections
    -fno-builtin
    -fshort-enums
    -funsigned-char
    -mthumb
    -fstack-usage
    -mcmse
    $<$<COMPILE_LANGUAGE:C>:-std=c99>
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
)

add_link_options(
    --entry=Reset_Handler
    -specs=nano.specs
    -specs=nosys.specs
    LINKER:-check-sections
    LINKER:-fatal-warnings
    LINKER:--gc-sections
    LINKER:--no-wchar-size-warning
)

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)
