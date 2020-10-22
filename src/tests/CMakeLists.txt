cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

#copy test data to build directory
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data/" 		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/data/")

# Finds boost.test or builds boost.test from github 
add_subdirectory(ext)

add_subdirectory(src)

add_subdirectory(end-to-end)