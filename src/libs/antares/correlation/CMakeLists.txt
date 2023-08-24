set(SRC_CORRELATION
        correlation.cpp
        antares/correlation/correlation.h
        antares/correlation/correlation.hxx
        )
source_group("correlation" FILES ${SRC_CORRELATION})

add_library(correlation ${SRC_CORRELATION})

target_link_libraries(correlation
        PRIVATE
        yuni-static-core
        inifile
        array
        )

target_include_directories(correlation
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/solver
        ${CMAKE_SOURCE_DIR}/libs/ #area -> antares/study/parameters/adq-patch-params.h
        ${CMAKE_SOURCE_DIR}/libs/antares/date #area -> antares/study/parameters/adq-patch-params.h -> date
        ${CMAKE_SOURCE_DIR}/libs/antares/writer #area -> antares/parts/thermal/cluster -> writer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )