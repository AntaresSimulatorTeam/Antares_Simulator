set(SRC_BENCHMARKING
        benchmarking.h
        timer.h
        timer.cpp
        info_collectors.h
        info_collectors.cpp
        file_content.h
        file_content.cpp
        )
source_group("misc\\benchmarking" FILES ${SRC_BENCHMARKING})

add_library(benchmarking
        ${SRC_BENCHMARKING}
        )

target_include_directories(benchmarking
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs
        )

target_link_libraries(benchmarking
        PRIVATE
        yuni-static-core
        inifile
        result_writer
        )
