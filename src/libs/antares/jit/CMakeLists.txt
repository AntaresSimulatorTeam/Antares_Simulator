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

target_include_directories(jit
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(jit
        PRIVATE
        yuni-static-core
        PUBLIC
        logs #templated function use logs
        )