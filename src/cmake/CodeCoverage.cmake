#
# Copyright (c) 2018, RTE (http://www.rte-france.com)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

include(CMakeParseArguments)

# Compilation flags
set(COVERAGE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_CXX_FLAGS}")

# Linker flags
set(COVERAGE_LINKER_FLAGS "--coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${COVERAGE_LINKER_FLAGS}")

function(code_coverage)
    set(options NONE)
    set(oneValueArgs NAME OUTPUT_DIR)
    set(multiValueArgs EXCLUDES EXCLUDE_DIRS)
    cmake_parse_arguments(Coverage "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Assert gcov (or llvm-cov) and gcovr commands are available
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        find_program(GCOV_CMD gcov)
        if (NOT GCOV_CMD)
            message(FATAL_ERROR "gcov not found")
        endif ()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        find_program(LLVM_COV_CMD llvm-cov)
        if (NOT LLVM_COV_CMD)
            message(FATAL_ERROR "llvm-cov not found")
        endif ()
        set(GCOV_CMD "${LLVM_COV_CMD} gcov")
    else ()
        message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} not supported")
    endif ()

    find_program(GCOVR_CMD gcovr)
    if (NOT GCOVR_CMD)
        message(FATAL_ERROR "gcovr not found")
    endif ()

    if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
        message(WARNING "Code coverage results with an optimized build may be misleading")
    endif ()

    if ("${Coverage_OUTPUT_DIR}" STREQUAL "")
        message(FATAL_ERROR "OUTPUT_DIR undefined")
    endif ()

    foreach (exclude ${Coverage_EXCLUDES})
        list(APPEND GCOVR_OPTIONS "--exclude" "${exclude}")
    endforeach ()

    foreach (exclude ${Coverage_EXCLUDE_DIRS})
        list(APPEND GCOVR_OPTIONS "--exclude-directories" "${exclude}")
    endforeach ()

    add_custom_target(${Coverage_NAME}
        # Cleaning previous results
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${Coverage_OUTPUT_DIR}

        # Generating report
        COMMAND ${CMAKE_COMMAND} -E make_directory ${Coverage_OUTPUT_DIR}/reports
        COMMAND ${GCOVR_CMD} ${CMAKE_CURRENT_BINARY_DIR}
        --gcov-executable "${GCOV_CMD}"
        --root ${CMAKE_SOURCE_DIR}
        --keep --object-directory ${Coverage_OUTPUT_DIR}/reports
        --html --html-details --output ${Coverage_OUTPUT_DIR}/index.html --sonarqube ${Coverage_OUTPUT_DIR}/coverage.xml
        ${GCOVR_OPTIONS}

        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )

endfunction()