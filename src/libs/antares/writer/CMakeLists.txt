project(result-writer)

add_library(result_writer
        # Helper class
        private/ensure_queue_started.h
        ensure_queue_started.cpp

        # Generic factory (public API)
        include/antares/writer/writer_factory.h
        writer_factory.cpp

        # Add entry to zip, using a job queue (private)
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

add_library(table_writer
        csv_table_writer.cpp
        csv_table_writer.h
        include/antares/writer/i_table_writer.h
        include/antares/writer/table_format.h
        include/antares/writer/table_writer_factory.h
        parquet_table_writer.cpp
        parquet_table_writer.h
        table_writer_factory.cpp
)

add_library(Antares::result_writer ALIAS result_writer)
add_library(Antares::table_writer ALIAS table_writer)

target_include_directories(result_writer
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
)

target_include_directories(table_writer
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

find_package(Parquet REQUIRED)
find_package(Arrow REQUIRED)

if (TARGET Arrow::arrow_shared)
    set(ANTARES_ARROW_TARGET Arrow::arrow_shared)
elseif (TARGET Arrow::arrow_static)
    set(ANTARES_ARROW_TARGET Arrow::arrow_static)
elseif (TARGET Arrow::arrow)
    set(ANTARES_ARROW_TARGET Arrow::arrow)
else ()
    message(FATAL_ERROR "Could not find a suitable Arrow target")
endif ()

if (TARGET Parquet::parquet_shared)
    set(ANTARES_PARQUET_TARGET Parquet::parquet_shared)
elseif (TARGET Parquet::parquet_static)
    set(ANTARES_PARQUET_TARGET Parquet::parquet_static)
elseif (TARGET Parquet::parquet)
    set(ANTARES_PARQUET_TARGET Parquet::parquet)
else ()
    message(FATAL_ERROR "Could not find a suitable Parquet target")
endif ()

target_link_libraries(table_writer
        PRIVATE
        ${ANTARES_ARROW_TARGET}
        ${ANTARES_PARQUET_TARGET}
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
