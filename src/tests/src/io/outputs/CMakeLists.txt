include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(TestSimulationTable
        SRC
        test_main.cpp
        testSimulationTable.cpp
        LIBS
        mock-modeler-objects
        Antares::modeler-simulation-table
        Antares::tests::inmemory-modeler
        Boost::unit_test_framework)
