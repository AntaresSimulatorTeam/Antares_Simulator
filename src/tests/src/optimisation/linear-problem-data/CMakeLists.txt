include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(unit-tests-for-modeler-data-series
        SRC
        main.cpp
        testScenarioGroupRepo.cpp
        testTimeSeriesSet.cpp
        testDataSeriesRepo.cpp
        testLinearProblemData.cpp
        LIBS
        linear-problem-data-impl
        test_utils_unit)