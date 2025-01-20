include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(unit-tests-for-modeler-data-series
        SRC
        main.cpp
        testScenarioGroupRepo.cpp
        testTimeSeriesSet.cpp
        INCLUDE
        "${solver_modeler_data_series}"
        LIBS
        data-series-lib
        test_utils_unit)