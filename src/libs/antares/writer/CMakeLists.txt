project(result-writer)

include(GenerateExportHeader)

find_package(Parquet REQUIRED)
find_package(Arrow REQUIRED)

add_library(simulation-table-writers SHARED
        include/antares/writer/i_table_writer.h
        LegacySimulationTablesWriter.cpp
        i_table_writer.cpp
        include/antares/writer/table_writer_factory.h
        private/parquet_arrow_utils.h
        columnToArrowAdapter.cpp
        table_writer_factory.cpp
        parquet_table_writer.cpp
        private/parquet_table_writer.h
        csv_table_writer.cpp
        private/csv_table_writer.h
)

generate_export_header(simulation-table-writers
    BASE_NAME SIMULATION_TABLE_WRITERS
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/simulation-table-writers_export.h"
)

set_target_properties(simulation-table-writers PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

set_target_properties(simulation-table-writers PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON)


target_include_directories(simulation-table-writers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  # For the generated simulation-table-writers_export.h
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/private
)

target_link_libraries(simulation-table-writers
  PUBLIC
  Antares::simulation-table
  Antares::exception
  PRIVATE
  # Keep Arrow/Parquet inside lib simulation-table-writers to avoid re-linking their static
  # objects into consumers, which can double-register/destroy generated globals.
  "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Arrow::arrow_static,Arrow::arrow_shared>"
  "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Parquet::parquet_static,Parquet::parquet_shared>")

add_library(Antares::simulation-table-writers ALIAS simulation-table-writers)

add_library(result_writer
        ensure_queue_started.cpp
        immediate_file_writer.cpp
        in_memory_writer.cpp
        null_result_writer.cpp
        include/antares/writer/i_table_writer.h
        include/antares/writer/in_memory_writer.h # Public (for tests)
        include/antares/writer/null_result_writer.h
        include/antares/writer/result_format.h
        include/antares/writer/writer_factory.h
        private/ensure_queue_started.h
        private/immediate_file_writer.h
        private/zip_writer.h
        writer_factory.cpp
        zip_writer.cpp
)

add_library(Antares::result_writer ALIAS result_writer)

target_include_directories(result_writer
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  # For the generated simulation-table-writers_export.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
)

target_link_libraries(result_writer
        PUBLIC
        Antares::exception
        Antares::benchmarking
        yuni-static-core
        # simulation-table-writers is a SHARED library (DLL on Windows). Since result_writer
        # is STATIC, consumers inherit its shared library dependencies at link time.
        # Using PUBLIC ensures MSVC's project reference system copies the DLL to
        # all consuming targets' output directories.
        simulation-table-writers
        PRIVATE
        MINIZIP::minizip
        logs
        inifile
        io
        Antares::concurrency
)


install(TARGETS simulation-table-writers
        RUNTIME DESTINATION bin    # .dll on Windows
        LIBRARY DESTINATION bin    # .so on Linux, kept next to executables for runtime loading
        ARCHIVE DESTINATION lib    # .lib import library on Windows
)

install(DIRECTORY include/antares
        DESTINATION "include"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/simulation-table-writers_export.h"
        DESTINATION "include/antares/writer"
)
