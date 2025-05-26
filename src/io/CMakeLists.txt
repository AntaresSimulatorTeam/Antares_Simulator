add_subdirectory(inputs)
set(HEADERS
        include/antares/io/statistics.h
        include/antares/io/file.h
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
add_library(Antares::io ALIAS io)

target_link_libraries(io
        PRIVATE
        yuni-static-core
        logs
)

target_include_directories(io
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
