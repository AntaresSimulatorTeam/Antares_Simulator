set(SRC_MATRIX
        include/antares/array/column-matrix.h
        include/antares/array/matrix.h
        include/antares/array/matrix.hxx
        include/antares/array/matrix-io.h
        matrix.cpp
        matrix-io.cpp
)
source_group("array" FILES ${SRC_MATRIX})

add_library(array ${SRC_MATRIX})
add_library(Antares::array ALIAS array)

target_link_libraries(array
        PRIVATE
        Antares::io
        Antares::logs
        yuni-static-core
)

target_include_directories(array
                PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
