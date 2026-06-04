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
        Antares::modeler-simulation-table
        Antares::tests::inmemory-modeler
        Antares::modeler-lib
        Antares::load-modeler-files
        Antares::write-modeler-files
        ortools::ortools
        Boost::unit_test_framework)

target_compile_definitions(testOutputs PRIVATE
        CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)