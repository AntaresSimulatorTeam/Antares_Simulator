project(result-writer)
add_library(result_writer
        # Interface class (public API)
        i_writer.h

        # Helper class
        ensure_queue_started.h
        ensure_queue_started.cpp

        # Generic factory (public API)
        writer_factory.h
        writer_factory.cpp

        # Immediate file writer (private)
        immediate_file_writer.h
        immediate_file_writer.cpp

        # Add entry to zip, using a job queue (private)
        zip_writer.cpp
        zip_writer.h)

target_include_directories(result_writer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/libs
)

target_link_libraries(result_writer
        PRIVATE
        MINIZIP::minizip
        logs
        inifile
        benchmarking
        io
    )



