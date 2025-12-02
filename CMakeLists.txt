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
include(stm32ddrfw_util)
include(stm32mp2_lp_version)

message(STATUS "STM32_SOC_NAME          : ${STM32_SOC_NAME}")
message(STATUS "STM32_DDR_TYPE          : ${STM32_DDR_TYPE}")
message(STATUS "STM32_DDR_FREQ          : ${STM32_DDR_FREQ}kHz (${STM32_DDR_FREQ_Mhz}MHz)")
message(STATUS "STM32_DDR_SIZE          : ${STM32_DDR_SIZE}bytes (${STM32_DDR_SIZE_Gb}Gb)")
message(STATUS "STM32MP2_LP_RETRAM_BASE : ${STM32MP2_LP_RETRAM_BASE}")
message(STATUS "STM32MP2_LP_SHARED_DATA : ${STM32MP2_LP_SHARED_DATA}")

#-------------------------------------------------------------------------------
project(${STM32MP2_LP_TARGET} C ASM)

# Default build type (RelWithDebInfo)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

add_executable(${STM32MP2_LP_TARGET})

set(STM32MP2_LP_CUBE_HAL    ${STM32MP2_LP_CUBE_PATH}/Drivers/STM32MP2xx_HAL_Driver)
set(STM32MP2_LP_DDR_HAL     ${STM32MP2_LP_DDRFW_UTIL_PATH}/Drivers/STM32MP2xx_HAL_Driver/)
set(STM32MP2_LP_INC_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/interface/include/)
set(STM32MP2_LP_SRC_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/interface/src/)

target_sources(${STM32MP2_LP_TARGET}
  PRIVATE
    ${STM32MP2_LP_SRC_DIR}/startup_stm32mp2xx_m33.S
    ${STM32MP2_LP_SRC_DIR}/main.c
    ${STM32MP2_LP_SRC_DIR}/syscalls.c
    ${STM32MP2_LP_SRC_DIR}/stm32mp2xx_it.c
    ${STM32MP2_LP_SRC_DIR}/system_stm32mp2xx.c
    ${STM32MP2_LP_CUBE_HAL}/Src/stm32mp2xx_hal.c
    ${STM32MP2_LP_CUBE_HAL}/Src/stm32mp2xx_hal_cortex.c
    ${STM32MP2_LP_CUBE_HAL}/Src/stm32mp2xx_hal_gpio.c
    ${STM32MP2_LP_CUBE_HAL}/Src/stm32mp2xx_hal_pwr.c
    ${STM32MP2_LP_CUBE_HAL}/Src/stm32mp2xx_hal_pwr_ex.c
    ${STM32MP2_LP_CUBE_HAL}/Src/stm32mp2xx_hal_rcc.c
    ${STM32MP2_LP_CUBE_HAL}/Src/stm32mp2xx_hal_rcc_ex.c

    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_firmware_ddr_pmu_train.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_calcmb.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_c_initphyconfig.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_d_loadimem.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_f_loaddmem.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_g_execfw.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_globals.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_i_loadpieimage.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_initstruct.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_isdbytedisabled.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_loadpieprodcode.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_mapdrvstren.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_progcsrskiptrain.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_reginterface.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_restore_sequence.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_sequence.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_softsetmb.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_usercustom_customposttrain.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_usercustom_custompretrain.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_usercustom_g_waitfwdone.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_usercustom_saveretregs.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr_ddrphy_phyinit_writeoutmem.c
    ${STM32MP2_LP_DDR_HAL}/Src/stm32mp2xx_hal_ddr.c
)

target_link_options(${STM32MP2_LP_TARGET}
  PRIVATE
    -T${STM32MP2_LP_LD_SCRIPT}
    -Wl,--defsym=RETRAM_BASE=${STM32MP2_LP_RETRAM_BASE}
    -Wl,-Map=${STM32MP2_LP_OUT_DIR}/${STM32MP2_LP_OUT_MAP}
    -Wl,--section-start=.shared_data_section=${STM32MP2_LP_SHARED_DATA}
    -Wl,--no-warn-rwx-segments
)

# Generate version
configure_file(${STM32MP2_LP_INC_DIR}/version.h.in
               ${CMAKE_BINARY_DIR}/generated/include/version.h)

target_include_directories(${STM32MP2_LP_TARGET}
  PRIVATE
    ${STM32MP2_LP_INC_DIR}
    ${STM32MP2_LP_INC_API_DIR}
    ${CMAKE_BINARY_DIR}/generated/include
)

target_compile_definitions(${STM32MP2_LP_TARGET}
  PRIVATE
    STM32MP2_LP_FW_SHARED_DATA_SECTION
    # flag for HAL configuration
    ${STM32_SOC_NAME}
    CORE_CM33
    STM32_M33TDCID
    # flag for HAL DDR driver configuration
    $<$<BOOL:${STM32MP2_LP_HALT_ON_ERROR}>:STM32MP2_LP_HALT_ON_ERROR>
    STM32MP_DDR_DUAL_AXI_PORT
    ${STM32MP2_DDR_OPTION}
    DDR_FREQ=${STM32_DDR_FREQ_Mhz}
    DDR_SIZE_Gb=${STM32_DDR_SIZE_Gb}
    ${STM32_DDR_TYPE}
)

target_link_libraries(${STM32MP2_LP_TARGET}
  PRIVATE
    stm32cube::header
    stm32ddrfw_util::header
    ${STM32MP2_LP_TARGET}_api
)

set_target_properties(${STM32MP2_LP_TARGET}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${STM32MP2_LP_OUT_DIR}"
)

# Share header with caller
add_library(${STM32MP2_LP_TARGET}_api INTERFACE)

target_include_directories(${STM32MP2_LP_TARGET}_api
  INTERFACE
    ${STM32MP2_LP_INC_API_DIR})

target_sources(${STM32MP2_LP_TARGET}_api
  INTERFACE
    ${STM32MP2_LP_SRC_API_DIR}/stm32mp2_lp_fw_api.c)

target_compile_definitions(${STM32MP2_LP_TARGET}_api
  INTERFACE
    STM32MP2_LP_FW_SHARED_ADDRESS=${STM32MP2_LP_SHARED_DATA}
)

add_library(${STM32MP2_LP_TARGET}::api ALIAS ${STM32MP2_LP_TARGET}_api)

# Execute post-build to print size, and create hex and bin
add_custom_command(
  OUTPUT ${STM32MP2_LP_OUT_DIR}/${STM32MP2_LP_OUT_BIN}
  DEPENDS ${STM32MP2_LP_TARGET}
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${STM32MP2_LP_TARGET}>
  COMMAND ${CMAKE_OBJCOPY} -j .text -j .ARM.extab -j .ARM.exidx -j .data -O binary $<TARGET_FILE:${STM32MP2_LP_TARGET}> ${STM32MP2_LP_OUT_DIR}/${STM32MP2_LP_OUT_BIN}
  COMMENT "generate low power firmware binary"
)

add_custom_target(${STM32MP2_LP_TARGET}_bin ALL
  DEPENDS ${STM32MP2_LP_OUT_DIR}/${STM32MP2_LP_OUT_BIN}
)
