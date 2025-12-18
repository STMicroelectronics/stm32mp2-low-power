#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT EXISTS ${CMSIS_CORE_PATH})

  message(STATUS "stm32mp2 low power cmsis_core fetching...")

  include(FetchContent)
  FetchContent_Declare(
    cmsis_core
    GIT_REPOSITORY ${CMSIS_CORE_GIT_REMOTE}
    GIT_TAG        ${CMSIS_CORE_VERSION}
    GIT_PROGRESS   TRUE
  )
  FetchContent_MakeAvailable(cmsis_core)

  set(CMSIS_CORE_PATH ${cmsis_core_SOURCE_DIR})

  file(INSTALL
    DESTINATION ${CMSIS_CORE_PATH}
    TYPE FILE
    FILES "${CMAKE_CURRENT_LIST_DIR}/cmsis_core_CMakeLists.txt"
    RENAME "CMakeLists.txt"
  )
endif()

add_subdirectory(${CMSIS_CORE_PATH} cmsis_core)
