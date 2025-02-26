include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)
add_boost_test(test-unfeasible-problem-analyzer
  SRC
  test-unfeasible-problem-analyzer.cpp
  LIBS
  infeasible_problem_analysis
  # Until issue here is resolved, link the whole archive: https://github.com/google/or-tools/discussions/4538
  # TODO: keep an eye out for bug resolution in future versions of ortools (> 9.12)
  $<LINK_LIBRARY:WHOLE_ARCHIVE,ortools::ortools>)
