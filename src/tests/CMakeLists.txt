cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

# Defining TESTING preprocessor variable at compile time
# This variable will soon be useless and droped. 
add_definitions(-DTESTING)

# Finds boost.test or builds boost.test from github 
add_subdirectory(ext)

add_subdirectory(src)
