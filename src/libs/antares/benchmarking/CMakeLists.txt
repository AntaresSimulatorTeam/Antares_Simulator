set(SRC_BENCHMARKING
        timer.cpp
        DurationCollector.cpp
        include/antares/benchmarking/file_content.h
        include/antares/benchmarking/timer.h
        include/antares/benchmarking/DurationCollector.h
        file_content.cpp
)
source_group("misc\\benchmarking" FILES ${SRC_BENCHMARKING})

add_library(benchmarking
        ${SRC_BENCHMARKING}
        )
add_library(Antares::benchmarking ALIAS benchmarking)

target_include_directories(benchmarking
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(benchmarking
        PRIVATE
        yuni-static-core
        inifile
        )
