add_subdirectory(UI)

set(SRC_AREAS
        constants.h
        area.h
        area.hxx
        scratchpad.h
        scratchpad.cpp
        area.cpp
        links.cpp
        links.h
        store-timeseries-numbers.cpp
        store-timeseries-numbers.h
        store-timeseries-numbers.hxx
        list.cpp
        list.hxx
        )
source_group("study\\area" FILES ${SRC_AREAS})

add_library(area
        ${SRC_AREAS})

target_include_directories(area
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/solver/writer
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(area
        PRIVATE
        yuni-static-core
        result_writer
        )