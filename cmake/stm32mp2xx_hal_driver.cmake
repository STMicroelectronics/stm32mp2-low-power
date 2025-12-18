#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT EXISTS ${STM32MP2XX_HAL_DRIVER_PATH})

  message(STATUS "stm32mp2 low power stm32mp2xx_hal_driver fetching...")

  include(FetchContent)
  FetchContent_Declare(
    stm32mp2xx_hal_driver
    GIT_REPOSITORY ${STM32MP2XX_HAL_DRIVER_GIT_REMOTE}
    GIT_TAG        ${STM32MP2XX_HAL_DRIVER_VERSION}
    GIT_PROGRESS   TRUE
  )
  FetchContent_MakeAvailable(stm32mp2xx_hal_driver)

  set(STM32MP2XX_HAL_DRIVER_PATH ${stm32mp2xx_hal_driver_SOURCE_DIR})

  file(INSTALL
    DESTINATION ${STM32MP2XX_HAL_DRIVER_PATH}
    TYPE FILE
    FILES "${CMAKE_CURRENT_LIST_DIR}/stm32mp2xx_hal_driver_CMakeLists.txt"
    RENAME "CMakeLists.txt"
  )
endif()

add_subdirectory(${STM32MP2XX_HAL_DRIVER_PATH} stm32mp2xx_hal_driver)
