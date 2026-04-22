include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# -----------------
# testOutputs
# -----------------
add_boost_test(testOutputs
        SRC
        test_main.cpp
        testSimulationTable.cpp
        testMpsWriter.cpp
        LIBS
        mock-modeler-objects
        Antares::simu-table-generator
        Antares::tests::inmemory-modeler
        Antares::modeler-lib
        Antares::load-modeler-files
        Antares::write-modeler-files
        Antares::simulation-table-writers
        ortools::ortools
        Boost::unit_test_framework)

target_compile_definitions(testOutputs PRIVATE
        CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

copy_project_shared_lib(Antares::simulation-table-writers testOutputs)

# ---------------------------------------
# test-parquet-simulation-table-writer
# ---------------------------------------
add_boost_test(test-parquet-simulation-table-writer
        SRC
        test-parquet-simulation-table-writer.cpp
        LIBS
        Antares::simulation-table-writers
        INCLUDE
        # gp : headers from target "simulation-table-writers" are needed to compile "test-parquet-simulation-table-writer"
        # gp : but they can't be located by this target, so we add the following instruction.
        "${CMAKE_SOURCE_DIR}/libs/antares/writer")

copy_project_shared_lib(Antares::simulation-table-writers test-parquet-simulation-table-writer)