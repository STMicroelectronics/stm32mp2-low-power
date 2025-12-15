#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT EXISTS ${STM32MP2_LP_CMSIS_CORE_PATH})

  message(STATUS "stm32mp2 low power cmsis core fetching...")

  include(FetchContent)
  FetchContent_Declare(
    stm32cube_cmsis_core
    GIT_REPOSITORY ${STM32MP2_LP_CMSIS_CORE_GIT_REMOTE}
    GIT_TAG        ${STM32MP2_LP_CMSIS_CORE_VERSION}
    GIT_PROGRESS   TRUE
  )
  FetchContent_MakeAvailable(stm32cube_cmsis_core)

  set(STM32MP2_LP_CMSIS_CORE_PATH ${stm32cube_cmsis_core_SOURCE_DIR})

  file(INSTALL
    DESTINATION ${STM32MP2_LP_CMSIS_CORE_PATH}
    TYPE FILE
    FILES "${CMAKE_CURRENT_LIST_DIR}/stm32cube_cmsis_core_CMakeLists.txt"
    RENAME "CMakeLists.txt"
  )
endif()

add_subdirectory(${STM32MP2_LP_CMSIS_CORE_PATH} stm32cube_cmsis_core)
