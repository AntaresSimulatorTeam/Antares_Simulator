include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(unit-tests-for-modeler-api
        SRC
        test_main.cpp
        testLinearProblemBuilder.cpp
        testLinearProblemMpsolverImpl.cpp
        LIBS
        Antares::linear-problem-mpsolver-impl
        linear-problem-data-impl)

