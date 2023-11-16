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
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)