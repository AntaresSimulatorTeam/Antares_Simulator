set(SRC_BENCHMARKING
        timer.cpp
        info_collectors.cpp
        antares/benchmarking/file_content.h
        antares/benchmarking/timer.h
        antares/benchmarking/info_collectors.h
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
