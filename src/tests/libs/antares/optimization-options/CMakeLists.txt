include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(TestOptimizationOptions
        SRC
        test_main.cpp
        test_optimization_options.cpp
        LIBS
        Antares::optimization-options
        Antares::checks
        Antares::exception)
