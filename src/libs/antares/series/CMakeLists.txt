add_library(series)
add_library(Antares::series ALIAS series)

set(SRC_STUDY_PART_SERIES
        include/antares/series/series.h
        series.cpp
)
target_sources(series PUBLIC ${SRC_STUDY_PART_SERIES})

target_link_libraries(series PUBLIC Antares::array)

target_include_directories(series
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
