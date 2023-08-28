project(result-writer)
add_library(result_writer
        # Interface class (public API)
        antares/writer/i_writer.h
        antares/writer/result_format.h

        # Helper class
        private/ensure_queue_started.h
        ensure_queue_started.cpp

        # Generic factory (public API)
        antares/writer/writer_factory.h
        writer_factory.cpp

        # Immediate file writer (private)
        private/immediate_file_writer.h
        immediate_file_writer.cpp

        # Add entry to zip, using a job queue (private)
        zip_writer.cpp
        private/zip_writer.h
        antares/writer/result_format.h
)

target_include_directories(result_writer
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(result_writer
        PRIVATE
        Antares::benchmarking
        yuni-static-core
        MINIZIP::minizip
        logs
        inifile
        io
    )



