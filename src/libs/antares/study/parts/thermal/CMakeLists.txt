set(SRC_STUDY_PART_THERMAL
        container.h
        container.hxx
        container.cpp
        prepro.h
        prepro.hxx
        prepro.cpp
        ecoInput.h
        ecoInput.cpp
        cluster.h
        cluster.hxx
        cluster.cpp
        cluster_list.h
        cluster_list.cpp
        pollutant.h
        pollutant.cpp
        )
source_group("study\\part\\thermal" FILES ${SRC_STUDY_PART_THERMAL})

add_library(thermal
        ${SRC_STUDY_PART_THERMAL}
        )

target_include_directories(thermal
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/solver/writer
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(thermal
        PRIVATE
        yuni-static-core
        )