#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT EXISTS ${STM32MP2_LP_CUBE_PATH})

  message(STATUS "stm32mp2 low power cube fetching...")

  include(FetchContent)
  FetchContent_Declare(
    stm32cube
    GIT_REPOSITORY ${STM32MP2_LP_CUBE_GIT_REMOTE}
    GIT_TAG        ${STM32MP2_LP_CUBE_VERSION}
    GIT_PROGRESS   TRUE
  )
  FetchContent_MakeAvailable(stm32cube)

  set(STM32MP2_LP_CUBE_PATH ${stm32cube_SOURCE_DIR})

  file(INSTALL
    DESTINATION ${STM32MP2_LP_CUBE_PATH}
    TYPE FILE
    FILES "${CMAKE_CURRENT_LIST_DIR}/stm32cube_CMakeLists.txt"
    RENAME "CMakeLists.txt"
  )
endif()

add_subdirectory(${STM32MP2_LP_CUBE_PATH} stm32cube)
