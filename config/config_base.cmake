#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
#toolchain
set(CROSS_COMPILE               arm-none-eabi     CACHE STRING    "Cross-compilation triplet")
set(STM32MP2_LP_TOOLCHAIN_FILE  toolchain_GNUARM  CACHE FILEPATH  "Path to stm32mp2_low_power compiler toolchain file")

#STM32Cube repository
set(STM32MP2XX_HAL_DRIVER_GIT_REMOTE "https://github.com/STMicroelectronics/stm32mp2xx-hal-driver.git" CACHE STRING "URL (or path) to retrieve stm32mp2xx-hal-driver package")
set(STM32MP2XX_HAL_DRIVER_VERSION    "v1.3.0"          CACHE STRING  "version of stm32mp2xx-hal-driver package")
set(STM32MP2XX_HAL_DRIVER_PATH       "download"        CACHE PATH  "Path to stm32mp2xx-hal-driver (or DOWNLOAD to fetch automatically)")

set(CMSIS_DEVICE_MP2_GIT_REMOTE      "https://github.com/STMicroelectronics/cmsis-device-mp2.git" CACHE STRING "URL (or path) to retrieve cmsis-device-mp2 package")
set(CMSIS_DEVICE_MP2_VERSION         "v1.3.0"          CACHE STRING  "version of cmsis-device-mp2 package")
set(CMSIS_DEVICE_MP2_PATH            "download"        CACHE PATH  "Path to cmsis-device-mp2: STM32CubeMP2 CMSIS Device MPU (or DOWNLOAD to fetch automatically)")

set(CMSIS_CORE_GIT_REMOTE            "https://github.com/STMicroelectronics/cmsis-core.git" CACHE STRING "URL (or path) to retrieve cmsis-core package")
set(CMSIS_CORE_VERSION               "v5.9.0"          CACHE STRING  "version of cmsis-core package")
set(CMSIS_CORE_PATH                  "download"        CACHE PATH  "Path to cmsis-core (or DOWNLOAD to fetch automatically)")

#output
set(STM32MP2_LP_TARGET          stm32mp2_lp_fw                  CACHE STRING  "target name of stm32mp2 low power")
set(STM32MP2_LP_OUT_DIR         ${CMAKE_CURRENT_BINARY_DIR}/bin CACHE PATH  "stm32mp2 low power output dir")
set(STM32MP2_LP_OUT_BIN         ${STM32MP2_LP_TARGET}.bin       CACHE PATH  "stm32mp2 low power output binary")
set(STM32MP2_LP_OUT_MAP         ${STM32MP2_LP_TARGET}.map       CACHE PATH  "stm32mp2 low power output map")

# HAL and DDR configuration
set(STM32_SOC_NAME      STM32MP257Fxx        CACHE STRING "SOC identifier: STM32MP257Fxx - STM32MP211Axx")
set(STM32_DDR_TYPE      STM32MP_DDR4_TYPE    CACHE STRING "DDR type, can be STM32MP_DDR3_TYPE, STM32MP_DDR4_TYPE or STM32MP_LPDDR4_TYPE")
set(STM32_DDR_FREQ      1200000              CACHE STRING "DDR frequency in KHZ")
set(STM32_DDR_SIZE      4294967296           CACHE STRING "DDR size in bytes")

#internal organization
set(STM32MP2_LP_SRC_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(STM32MP2_LP_INC_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(STM32MP2_LP_LD_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/linker_scripts)

#configuration
set(STM32MP2_LP_HALT_ON_ERROR   OFF               CACHE BOOL    "On fatal errors in the firmware, halt instead of rebooting.")

#internal var
set(STM32MP2_LP_LD_SCRIPT       ${STM32MP2_LP_LD_DIR}/stm32mp2xx_RETRAM_m33.ld)

math(EXPR STM32_DDR_SIZE_Gb "${STM32_DDR_SIZE} * 8 / 1024 / 1024 / 1024 ")
math(EXPR STM32_DDR_FREQ_Mhz "${STM32_DDR_FREQ} / 1000")

string(REGEX MATCH "^STM32MP2[135]" STM32_SOC_FAMILY ${STM32_SOC_NAME})
if(${STM32_SOC_NAME} MATCHES "^STM32MP21")
    set(STM32MP2_LP_RETRAM_BASE 0x0e040000)
    set(STM32MP2_LP_SHARED_DATA 0x0e051800)
elseif(${STM32_SOC_NAME} MATCHES "^STM32MP23" OR ${STM32_SOC_NAME} MATCHES "^STM32MP25")
    set(STM32MP2_LP_RETRAM_BASE 0x0e080000)
    set(STM32MP2_LP_SHARED_DATA 0x0e091800)
else()
    message(FATAL_ERROR "Unknown or unsupported STM32MP2 chip define")
endif()

if(${STM32_SOC_NAME} MATCHES "^STM32MP21" OR ${STM32_SOC_NAME} MATCHES "^STM32MP23")
    set(STM32MP2_DDR_OPTION     STM32MP_DDR_16_BIT_INTERFACE)
endif()
