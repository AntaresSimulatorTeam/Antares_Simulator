set(SRC_JIT
        # JIT (Just in Time)
        jit.h
        jit.hxx
        jit.cpp
        )
source_group("misc\\jit" FILES ${SRC_JIT})

add_library(jit
        ${SRC_JIT}
        )

target_link_libraries(jit
        PRIVATE
        logs
        yuni-static-core
        )

target_include_directories(jit
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/libs
        )