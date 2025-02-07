include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)
add_boost_test(test-unfeasible-problem-analyzer
  SRC
  test-unfeasible-problem-analyzer.cpp
  LIBS
  infeasible_problem_analysis
  ortools::ortools)
