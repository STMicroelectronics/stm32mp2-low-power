#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT EXISTS ${STM32MP2_LP_DDRFW_UTIL_PATH})

	message(STATUS "stm32mp2 low power ddrfw util fetching...")

	include(FetchContent)
	FetchContent_Declare(
		stm32ddrfw_util
		GIT_REPOSITORY ${STM32MP2_LP_DDRFW_UTIL_GIT_REMOTE}
		GIT_TAG        ${STM32MP2_LP_DDRFW_UTIL_VERSION}
		GIT_PROGRESS   TRUE
	)
	FetchContent_MakeAvailable(stm32ddrfw_util)

	set(STM32MP2_LP_DDRFW_UTIL_PATH ${stm32ddrfw_util_SOURCE_DIR})

	file(INSTALL
		DESTINATION ${STM32MP2_LP_DDRFW_UTIL_PATH}
		TYPE FILE
		FILES "${CMAKE_CURRENT_LIST_DIR}/stm32ddrfw_util_CMakeLists.txt"
		RENAME "CMakeLists.txt"
	)
endif()

add_subdirectory(${STM32MP2_LP_DDRFW_UTIL_PATH} stm32ddrfw_util)
