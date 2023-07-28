set(SRC_STUDY_AREA_UI
        ui.h
        ui.hxx
        ui.cpp
        )
source_group("study\\area\\ui" FILES ${SRC_STUDY_AREA_UI})

add_library(area_ui
        ${SRC_STUDY_AREA_UI})

target_include_directories(area_ui
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(area_ui
        PRIVATE
        yuni-static-core
        logs
        )