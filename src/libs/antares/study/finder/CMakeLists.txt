set(SRC_STUDY_FINDER
        finder.cpp
        finder.h
        )
source_group("study\\finder" FILES ${SRC_STUDY_FINDER})

add_library(finder
        ${SRC_STUDY_FINDER})

target_include_directories(finder
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(finder
        PRIVATE
        yuni-static-core
        result_writer
        )