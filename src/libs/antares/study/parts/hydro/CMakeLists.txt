set(SRC_STUDY_PART_HYDRO
        container.h
        container.cpp
        series.h
        series.hxx
        series.cpp
        prepro.h
        prepro.cpp
        allocation.h
        allocation.hxx
        allocation.cpp
        )
source_group("study\\part\\hydro" FILES ${SRC_STUDY_PART_HYDRO})

add_library(hydro
        ${SRC_STUDY_PART_HYDRO}
        )

target_include_directories(hydro
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/solver/writer
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(hydro
        PRIVATE
        yuni-static-core
        )