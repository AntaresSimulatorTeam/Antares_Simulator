set(SRC_STUDY_PART_COMMON
        series.h
        series.hxx
        series.cpp
        cluster.cpp
        cluster.h
        cluster_list.h
        cluster_list.cpp
        )
source_group("study\\part\\common" FILES ${SRC_STUDY_PART_COMMON})

add_library(parts_common
        ${SRC_STUDY_PART_COMMON}
        )

target_include_directories(parts_common
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/solver/writer
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(parts_common
        PRIVATE
        yuni-static-core
        )