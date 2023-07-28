set(SRC_STUDY_PART_RENEWABLE
        container.h
        container.hxx
        container.cpp
        cluster.h
        cluster.cpp
        cluster_list.h
        cluster_list.cpp
        )
source_group("study\\part\\renewable" FILES ${SRC_STUDY_PART_RENEWABLE})

add_library(renewable
        ${SRC_STUDY_PART_RENEWABLE}
        )

target_include_directories(renewable
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/solver/writer
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(renewable
        PRIVATE
        yuni-static-core
        )