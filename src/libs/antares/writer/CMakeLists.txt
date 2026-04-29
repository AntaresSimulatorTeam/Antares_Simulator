project(result-writer)

include(GenerateExportHeader)

find_package(Parquet REQUIRED)
find_package(Arrow REQUIRED)

add_library(parquet_writer SHARED
        parquet_table_writer.cpp
        parquet_table_writer.h)

generate_export_header(parquet_writer)

set_target_properties(parquet_writer PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

target_include_directories(parquet_writer
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  # For the generated parquet_writer_export.h
)

target_link_libraries(parquet_writer
  PRIVATE
  "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Arrow::arrow_static,Arrow::arrow_shared>"
  "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Parquet::parquet_static,Parquet::parquet_shared>")

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

target_link_libraries(result_writer
        PUBLIC
        Antares::benchmarking
        yuni-static-core
        # parquet_writer is a SHARED library (DLL on Windows). Since result_writer
        # is STATIC, consumers inherit its shared library dependencies at link time.
        # Using PUBLIC ensures MSVC's project reference system copies the DLL to
        # all consuming targets' output directories.
        parquet_writer
        PRIVATE
        MINIZIP::minizip
        logs
        inifile
        io
        Antares::concurrency
)


install(TARGETS parquet_writer
        RUNTIME DESTINATION bin    # .dll on Windows
        LIBRARY DESTINATION lib    # .so on Linux
        ARCHIVE DESTINATION lib    # .lib import library on Windows
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
