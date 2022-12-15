set(SRC_JIT
        # JIT (Just in Time)
        include/antares/jit/jit.h
        include/antares/jit/jit.hxx
        jit.cpp
        )
source_group("misc\\jit" FILES ${SRC_JIT})

add_library(jit
        ${SRC_JIT}
        )

target_include_directories(jit
                PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(jit
        PRIVATE
        yuni-static-core
        PUBLIC
        logs #templated function use logs
        )