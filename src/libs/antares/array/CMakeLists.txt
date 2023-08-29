set(SRC_MATRIX
        antares/array/matrix.h
        antares/array/matrix.hxx
        matrix.cpp
        )
source_group("array" FILES ${SRC_MATRIX})

add_library(array ${SRC_MATRIX})
add_library(Antares::array ALIAS array)

target_link_libraries(array
        PRIVATE
        yuni-static-core
        PUBLIC
        io #matrix.hxx
        jit #jit.hxx require logs
        Antares::memory
        )

target_include_directories(array
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )