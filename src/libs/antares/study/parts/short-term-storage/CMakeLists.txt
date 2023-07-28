set(SRC_STUDY_PART_SHORT_TERM_STORAGE
        container.h
        container.cpp
        properties.h
        properties.cpp
        series.cpp
        series.h
        cluster.h
        cluster.cpp
        STStorageOutputCaptions.h
        STStorageOutputCaptions.cpp
        )
source_group("study\\part\\short-term-storage" FILES ${SRC_STUDY_PART_SHORT_TERM_SOTRAGE})

add_library(short_term_storage
        ${SRC_STUDY_PART_SHORT_TERM_STORAGE}
        )

target_include_directories(short_term_storage
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/solver/writer
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(short_term_storage
        PRIVATE
        yuni-static-core
        logs
        inifile
        )