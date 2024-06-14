project(result-writer)

add_library(result_writer
        # Helper class
        private/ensure_queue_started.h
        ensure_queue_started.cpp

        # Generic factory (public API)
        include/antares/writer/writer_factory.h
        writer_factory.cpp

        # Immediate file writer (private)
        private/immediate_file_writer.h
        immediate_file_writer.cpp

        # Add entry to zip, using a job queue (private)
        zip_writer.cpp
        private/zip_writer.h

        in_memory_writer.cpp
        include/antares/writer/in_memory_writer.h # Public (for tests)
        include/antares/writer/result_format.h
)

add_library(Antares::result_writer ALIAS result_writer)

target_include_directories(result_writer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
)

target_link_libraries(result_writer
        PUBLIC
        Antares::benchmarking
        yuni-static-core
        PRIVATE
        MINIZIP::minizip
        logs
        inifile
        io
        Antares::concurrency
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
