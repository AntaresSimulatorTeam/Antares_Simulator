project(result-writer)

add_library(result_writer
        # Helper class


        # Add entry to zip, using a job queue (private)
        # Generic factory (public API)
        # Immediate file writer (private)
        # Table writers (public API + implementations)
        csv_table_writer.cpp
        csv_table_writer.h
        ensure_queue_started.cpp
        immediate_file_writer.cpp
        in_memory_writer.cpp
        include/antares/writer/i_table_writer.h
        include/antares/writer/in_memory_writer.h # Public (for tests)
        include/antares/writer/result_format.h
        include/antares/writer/table_format.h
        include/antares/writer/table_writer_factory.h
        include/antares/writer/writer_factory.h
        parquet_table_writer.cpp
        parquet_table_writer.h
        private/ensure_queue_started.h
        private/immediate_file_writer.h
        private/zip_writer.h
        table_writer_factory.cpp
        writer_factory.cpp
        zip_writer.cpp
)

add_library(Antares::result_writer ALIAS result_writer)

target_include_directories(result_writer
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
)

find_package(Parquet REQUIRED)
find_package(Arrow REQUIRED)

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
        Arrow::arrow_static
        Parquet::parquet_static
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
