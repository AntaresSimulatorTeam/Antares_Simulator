include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

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
        Antares::simulation-table-writers
        ortools::ortools
        Boost::unit_test_framework
        # Arrow/Parquet are linked transitively via Antares::simulation-table-writers.
        # Linking them directly here would duplicate the static library's global objects
        # (e.g. std::map<int, char const*> in parquet::format), causing a double-free
        # at program exit when both copies are destroyed.
        # The test's direct use of parquet::arrow::OpenFile resolves through the .so.
)

target_compile_definitions(testOutputs PRIVATE
        RESOURCES_DIR="${CMAKE_SOURCE_DIR}/resources"
)
# ---------------------------------------
# test-parquet-simulation-table-writer
# ---------------------------------------
add_boost_test(test-parquet-simulation-table-writer
        SRC
        test-parquet-simulation-table-writer.cpp
        LIBS
        Antares::simulation-table-writers
        "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Arrow::arrow_static,Arrow::arrow_shared>"
        "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Parquet::parquet_static,Parquet::parquet_shared>"
        INCLUDE
        # gp : headers from target "simulation-table-writers" are needed to compile "test-parquet-simulation-table-writer"
        # gp : but they can't be located by this target, so we add the following instruction.
        "${CMAKE_SOURCE_DIR}/src/libs/antares/writer")
