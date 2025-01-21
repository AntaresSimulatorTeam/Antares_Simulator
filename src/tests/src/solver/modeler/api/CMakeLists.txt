include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(unit-tests-for-modeler-api
        SRC
        test_main.cpp
        testModelerLinearProblemWithOrtools.cpp
        testModelerLPbuilder.cpp
        LIBS
        Antares::modeler-ortools-impl
        data-series-lib)

