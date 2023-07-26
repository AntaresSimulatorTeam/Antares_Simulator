set(SRC_MEMORY
        memory.h
        memory.hxx
        memory.cpp IMemoryUsage.h)

source_group("memory" FILES ${SRC_MEMORY})

add_library(memory
        ${SRC_MEMORY})

target_include_directories(memory
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        )

target_link_libraries(memory
        PRIVATE
        yuni-static-core
        )