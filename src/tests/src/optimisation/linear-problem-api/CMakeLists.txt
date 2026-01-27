include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test_structured_linear_problem
        SRC
        test_structured_linear_problem.cpp
        LIBS
        linear-problem-api
        structured-linear-problem)
