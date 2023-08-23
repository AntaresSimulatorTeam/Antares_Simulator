set(SRC_MATRIX
        antares/array/matrix.h
        antares/array/array1d.h
        antares/array/matrix.hxx
        matrix.cpp
        )
source_group("array" FILES ${SRC_MATRIX})

add_library(array ${SRC_MATRIX})

target_link_libraries(array
        PRIVATE
        yuni-static-core
        PUBLIC
        io #matrix.hxx
        jit #jit.hxx require logs
        )

target_include_directories(array
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs #memory usage
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )