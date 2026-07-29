project(result-writer)

find_package(Parquet REQUIRED)
find_package(Arrow REQUIRED)

add_library(simulation-table-writers
        LegacySimulationTablesWriter.cpp
        include/antares/writer/simulation_table_writer.h
        private/parquet_arrow_utils.h
        columnToArrowAdapter.cpp
        simulation_table_writer.cpp)

target_include_directories(simulation-table-writers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/private
)

target_link_libraries(simulation-table-writers
  PUBLIC
  Antares::simulation-table
  Antares::exception
  "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Arrow::arrow_static,Arrow::arrow_shared>"
  "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Parquet::parquet_static,Parquet::parquet_shared>")

add_library(Antares::simulation-table-writers ALIAS simulation-table-writers)

add_library(result_writer
        immediate_file_writer.cpp
        in_memory_writer.cpp
        null_result_writer.cpp
        include/antares/writer/in_memory_writer.h # Public (for tests)
        include/antares/writer/null_result_writer.h
        include/antares/writer/result_format.h
        include/antares/writer/writer_factory.h
        private/immediate_file_writer.h
        private/zip_writer.h
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
        Antares::exception
        Antares::benchmarking
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

