#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT EXISTS ${CMSIS_DEVICE_MP2_PATH})

  message(STATUS "stm32mp2 low power cmsis_device_mp2 fetching...")

  include(FetchContent)
  FetchContent_Declare(
    cmsis_device_mp2
    GIT_REPOSITORY ${CMSIS_DEVICE_MP2_GIT_REMOTE}
    GIT_TAG        ${CMSIS_DEVICE_MP2_VERSION}
    GIT_PROGRESS   TRUE
  )
  FetchContent_MakeAvailable(cmsis_device_mp2)

  set(CMSIS_DEVICE_MP2_PATH ${cmsis_device_mp2_SOURCE_DIR})

  file(INSTALL
    DESTINATION ${CMSIS_DEVICE_MP2_PATH}
    TYPE FILE
    FILES "${CMAKE_CURRENT_LIST_DIR}/cmsis_device_mp2_CMakeLists.txt"
    RENAME "CMakeLists.txt"
  )
endif()

add_subdirectory(${CMSIS_DEVICE_MP2_PATH} cmsis_device_mp2)
