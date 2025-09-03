#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
#toolchain
set(CROSS_COMPILE		arm-none-eabi			CACHE STRING	"Cross-compilation triplet")
set(STM32MP2_LP_TOOLCHAIN_FILE	toolchain_GNUARM		CACHE FILEPATH	"Path to stm32mp2_low_power compiler toolchain file")

#Cube repository
set(STM32MP2_LP_CUBE_GIT_REMOTE "https://github.com/STMicroelectronics/STM32CubeMP2.git" CACHE STRING "URL (or path) to retrieve stm32cubemp2 package")
set(STM32MP2_LP_CUBE_VERSION	"v1.2.0"			CACHE STRING	"version of stm32cubemp2 package")
set(STM32MP2_LP_CUBE_PATH	"download"			CACHE PATH	"Path to stm32cubemp2 (or DOWNLOAD to fetch automatically)")

set(STM32MP2_LP_SOC		"STM32MP257Fxx"			CACHE STRING	"define soc family used")
set(STM32MP2_LP_TARGET		stm32mp2_lp_fw			CACHE STRING	"target name of stm32mp2 low power")

#output
set(STM32MP2_LP_OUT_DIR		${CMAKE_CURRENT_BINARY_DIR}/bin CACHE PATH	"stm32mp2 low power output dir")
set(STM32MP2_LP_OUT_BIN		${STM32MP2_LP_TARGET}.bin	CACHE PATH	"stm32mp2 low power output binary")
set(STM32MP2_LP_OUT_MAP		${STM32MP2_LP_TARGET}.map	CACHE PATH	"stm32mp2 low power output map")

#internal organization
set(STM32MP2_LP_SRC_DIR		${CMAKE_CURRENT_SOURCE_DIR}/src)
set(STM32MP2_LP_INC_DIR		${CMAKE_CURRENT_SOURCE_DIR}/include)
set(STM32MP2_LL_LD_DIR		${CMAKE_CURRENT_SOURCE_DIR}/linker_scripts)

#internal var
set(STM32MP2_LP_CORE		CORE_CM33)
set(STM32MP2_LL_LD_SCRIPT	${STM32MP2_LL_LD_DIR}/stm32mp25xx_RETRAM_m33.ld)
set(STM32MP2_LP_SHARED_DATA 	0x0e090000)
