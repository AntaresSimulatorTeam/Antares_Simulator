set(SRC_CORRELATION
        correlation.cpp
        include/antares/correlation/correlation.h
        include/antares/correlation/correlation.hxx
        )
source_group("correlation" FILES ${SRC_CORRELATION})

add_library(correlation
        ${SRC_CORRELATION}
)
add_library(Antares::correlation ALIAS correlation)

target_link_libraries(correlation
        PRIVATE
        yuni-static-core
        Antares::inifile
        Antares::array
        Antares::study #correlation.h -> area.h
        Antares::locator
        )

target_include_directories(correlation
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/> #area -> antares/study/parameters/adq-patch-params.h
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/date> #area -> antares/study/parameters/adq-patch-params.h -> date
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/writer> #area -> antares/parts/thermal/cluster -> writer
                PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )