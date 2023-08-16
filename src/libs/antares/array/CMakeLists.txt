set(SRC_MATRIX
        array1d.h
        array1d.cpp
        matrix.h
        matrix.cpp
        matrix.hxx
        )
source_group("array" FILES ${SRC_MATRIX})

add_library(array ${SRC_MATRIX})

target_link_libraries(array
        PRIVATE
        yuni-static-core
        jit
        )

target_include_directories(array
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs
        )