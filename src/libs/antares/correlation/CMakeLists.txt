set(SRC_CORRELATION
        correlation.cpp
        correlation.h
        correlation.hxx
        )
source_group("correlation" FILES ${SRC_CORRELATION})

add_library(correlation ${SRC_CORRELATION})

target_link_libraries(correlation
        PRIVATE
        yuni-static-core
        result_writer
        )

target_include_directories(correlation
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs
        ${CMAKE_SOURCE_DIR}/libs/solver
        )