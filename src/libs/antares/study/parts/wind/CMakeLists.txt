set(SRC_STUDY_PART_WIND
        container.h
        container.cpp
        series.h
        series.hxx
        series.cpp
        prepro.h
        prepro.hxx
        prepro.cpp
        )
source_group("study\\part\\wind" FILES ${SRC_STUDY_PART_WIND})

add_library(wind
        ${SRC_STUDY_PART_WIND}
        )

target_include_directories(wind
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/solver/writer
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(wind
        PRIVATE
        yuni-static-core
        )