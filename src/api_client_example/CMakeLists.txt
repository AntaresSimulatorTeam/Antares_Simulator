cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable

set(CMAKE_CXX_STANDARD 20)

find_package(antlr4-runtime REQUIRED)
find_package(yaml-cpp REQUIRED)

PROJECT(API_client)

add_subdirectory(src)

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
