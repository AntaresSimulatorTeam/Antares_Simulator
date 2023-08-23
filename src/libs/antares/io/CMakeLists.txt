set(HEADERS
        antares/io/statistics.h
        antares/io/file.h
)
set(SRC_IO
        ${HEADERS}
        statistics.cpp
        file.cpp
        )
source_group("io" FILES ${SRC_IO})

add_library(io
        ${SRC_IO}
        )

target_link_libraries(io
        PRIVATE
        yuni-static-core
        logs
        )

target_include_directories(io
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )