set(SRC_BENCHMARKING
        timer.cpp
        info_collectors.cpp
        benchmarking/file_content.h
        benchmarking/timer.h
        benchmarking/info_collectors.h
        file_content.cpp
        )
source_group("misc\\benchmarking" FILES ${SRC_BENCHMARKING})

add_library(benchmarking
        ${SRC_BENCHMARKING}
        )

target_include_directories(benchmarking
        PUBLIC
        ${CMAKE_SOURCE_DIR}/libs #study
        ${CMAKE_SOURCE_DIR}/libs/antares/date #parameters -> date
        ${CMAKE_SOURCE_DIR}/libs/antares/array #links
        ${CMAKE_SOURCE_DIR}/libs/antares/logs #study -> binding constraint -> matrix -> jit
        ${CMAKE_SOURCE_DIR}/libs/antares/io #study -> binding constraint -> matrix
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(benchmarking
        PRIVATE
        yuni-static-core
        inifile
        result_writer
        )
