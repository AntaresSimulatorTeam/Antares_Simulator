add_library(series)
add_library(Antares::series ALIAS series)

set(SRC_STUDY_PART_SERIES
        include/antares/series/series.h
        series.cpp
)
target_sources(series
	PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/antares/series/series.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/series.cpp>
)

target_link_libraries(series PUBLIC Antares::array)

target_include_directories(series
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)