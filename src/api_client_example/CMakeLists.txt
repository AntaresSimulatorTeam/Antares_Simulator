cmake_minimum_required(VERSION 3.5)

PROJECT(API_client)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()