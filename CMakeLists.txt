#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(config_base)
include(${STM32MP2_LP_TOOLCHAIN_FILE})
include(stm32cube)

#-------------------------------------------------------------------------------
project(${STM32MP2_LP_TARGET} C ASM)

add_executable(${STM32MP2_LP_TARGET})

set(STM32MP2_LP_CUBE_HAL ${STM32MP2_LP_CUBE_PATH}/Drivers/STM32MP2xx_HAL_Driver)

target_sources(${STM32MP2_LP_TARGET}
	PRIVATE
		${STM32MP2_LP_SRC_DIR}/startup_stm32mp2xx_m33.S
		${STM32MP2_LP_SRC_DIR}/main.c
		${STM32MP2_LP_SRC_DIR}/syscalls.c
		${STM32MP2_LP_SRC_DIR}/stm32mp2xx_it.c
		${STM32MP2_LP_SRC_DIR}/system_stm32mp2xx.c
		${STM32MP2_LP_CUBE_HAL}/Src/stm32mp2xx_hal.c
)

target_link_options(${STM32MP2_LP_TARGET}
	PRIVATE
		-T${STM32MP2_LL_LD_SCRIPT}
		-Wl,-Map=${STM32MP2_LP_OUT_DIR}/${STM32MP2_LP_OUT_MAP}
)

target_include_directories(${STM32MP2_LP_TARGET}
	PRIVATE
		${STM32MP2_LP_INC_DIR}
)

target_compile_definitions(${STM32MP2_LP_TARGET}
	PRIVATE
		${STM32MP2_LP_CORE}
		${STM32MP2_LP_SOC}
)

target_link_libraries(${STM32MP2_LP_TARGET}
	PRIVATE
		stm32cube::header
)

set_target_properties(${STM32MP2_LP_TARGET}
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${STM32MP2_LP_OUT_DIR}"
)

# Execute post-build to print size, and create hex and bin
add_custom_command(
	OUTPUT ${STM32MP2_LP_OUT_DIR}/${STM32MP2_LP_OUT_BIN}
	DEPENDS ${STM32MP2_LP_TARGET}
	COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${STM32MP2_LP_TARGET}>
	COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${STM32MP2_LP_TARGET}> ${STM32MP2_LP_OUT_DIR}/${STM32MP2_LP_OUT_BIN}
	COMMENT "generate low power firmware binary"
)

add_custom_target(${STM32MP2_LP_TARGET}_bin ALL
    DEPENDS ${STM32MP2_LP_OUT_DIR}/${STM32MP2_LP_OUT_BIN}
)
