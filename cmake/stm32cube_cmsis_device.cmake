#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT EXISTS ${STM32MP2_LP_CMSIS_DEVICE_PATH})

  message(STATUS "stm32mp2 low power cmsis device fetching...")

  include(FetchContent)
  FetchContent_Declare(
    stm32cube_cmsis_device
    GIT_REPOSITORY ${STM32MP2_LP_CMSIS_DEVICE_GIT_REMOTE}
    GIT_TAG        ${STM32MP2_LP_CMSIS_DEVICE_VERSION}
    GIT_PROGRESS   TRUE
  )
  FetchContent_MakeAvailable(stm32cube_cmsis_device)

  set(STM32MP2_LP_CMSIS_DEVICE_PATH ${stm32cube_cmsis_device_SOURCE_DIR})

  file(INSTALL
    DESTINATION ${STM32MP2_LP_CMSIS_DEVICE_PATH}
    TYPE FILE
    FILES "${CMAKE_CURRENT_LIST_DIR}/stm32cube_cmsis_device_CMakeLists.txt"
    RENAME "CMakeLists.txt"
  )
endif()

add_subdirectory(${STM32MP2_LP_CMSIS_DEVICE_PATH} stm32cube_cmsis_device)
