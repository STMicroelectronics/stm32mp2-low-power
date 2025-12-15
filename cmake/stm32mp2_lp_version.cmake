#-------------------------------------------------------------------------------
# SPDX-License-Identifier: BSD-3-Clause
#-------------------------------------------------------------------------------

# The '_VERSION_MANUAL' is used for fallback when Git tags are not available
set(STM32MP2_LP_VERSION_MANUAL "0.5.0")

find_package(Git)
if(Git_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always --match "v[0-9]*\.[0-9]*\.[0-9]*"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE STM32MP2_LP_VERSION_FULL
        OUTPUT_STRIP_TRAILING_WHITESPACE)

# In a repository cloned with --no-tags option STM32MP2_LP_VERSION_FULL will be a hash
# only hence checking it for a tag format vX.Y.Z to accept as valid version.

    string(REGEX MATCH "^v[0-9]+\\.[0-9]+\\.[0-9]+" STM32MP2_LP_VERSION "${STM32MP2_LP_VERSION_FULL}")
    string(REGEX REPLACE "v" "" STM32MP2_LP_VERSION "${STM32MP2_LP_VERSION}")
endif()

if (STM32MP2_LP_VERSION STREQUAL "")
    set(STM32MP2_LP_VERSION_FULL "v${STM32MP2_LP_VERSION_MANUAL}")
    set(STM32MP2_LP_VERSION "${STM32MP2_LP_VERSION_MANUAL}")
    message(WARNING "Actual STM32MP2 LP firmware version is not available from Git repository. Settled to " ${STM32MP2_LP_VERSION_FULL})
endif()

# Check that manually set version is up to date
if (NOT STM32MP2_LP_VERSION_MANUAL STREQUAL STM32MP2_LP_VERSION)
    message(WARNING "STM32MP2_LP_VERSION_MANUAL mismatches to actual STM32MP2 LP firmware version. Please update STM32MP2_LP_VERSION_MANUAL in cmake/version.cmake")
endif()

message(STATUS "STM32MP2 LP firmware version: ${STM32MP2_LP_VERSION_FULL}")
