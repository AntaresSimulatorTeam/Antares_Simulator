set(SRC_IO
        statistics.h
        statistics.cpp
        file.h
        file.cpp
        )
source_group("io" FILES ${SRC_IO})

add_library(io
        ${SRC_IO}
        )

target_link_libraries(io
        PRIVATE
        yuni-static-core
        )

target_include_directories(io
        PRIVATE
        )