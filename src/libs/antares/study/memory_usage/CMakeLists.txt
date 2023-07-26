set(SRC_MEMORY_USAGE
        memory-usage.h
        memory-usage.cpp)

source_group("memory-usage" FILES ${SRC_MEMORY_USAGE})

add_library(memory-usage
        ${SRC_MEMORY_USAGE})

target_include_directories(memory-usage
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(memory-usage
        PRIVATE
        yuni-static-core
        libantares-core
        memory
        array
        )