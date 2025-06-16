cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable

PROJECT(API_client C CXX)

include(CheckCXXSourceCompiles)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckCXXCompilerFlag)
include(CMakeDependentOption)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

find_package(antlr4-runtime REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(fmt REQUIRED)


add_subdirectory(src)

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
