include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

find_package(Parquet REQUIRED)
find_package(Arrow REQUIRED)

# -----------------
# testOutputs
# -----------------
add_boost_test(testOutputs
        SRC
        test_main.cpp
        testSimulationTable.cpp
        testMpsWriter.cpp
        LIBS
        test_utils_unit
        mock-modeler-objects
        Antares::simu-table-generator
        Antares::tests::inmemory-modeler
        Antares::modeler-lib
        Antares::load-modeler-files
        Antares::write-modeler-files
        ortools::ortools
        Boost::unit_test_framework)

target_compile_definitions(testOutputs PRIVATE
        CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

# ---------------------------------------
# test-parquet-simulation-table-writer
# ---------------------------------------
add_boost_test(test-parquet-simulation-table-writer
        SRC
        test-parquet-simulation-table-writer.cpp
        LIBS
        Antares::parquet_writer
        "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Arrow::arrow_static,Arrow::arrow_shared>"
        "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Parquet::parquet_static,Parquet::parquet_shared>"
        INCLUDE
        # gp : headers from target "simulation-table-writers" are needed to compile "test-parquet-simulation-table-writer"
        # gp : but they can't be located by this target, so we add the following instruction.
        "${CMAKE_SOURCE_DIR}/libs/antares/writer")

copy_project_shared_lib(Antares::parquet_writer test-parquet-simulation-table-writer)

copy_project_shared_lib(parquet_writer testOutputs)
