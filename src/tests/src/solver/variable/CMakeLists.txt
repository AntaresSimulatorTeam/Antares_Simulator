include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-intermediate
  SRC test_intermediate.cpp
  LIBS antares-solver-variable)

add_boost_test(test-surveyresults
  SRC test_surveyresults.cpp
  LIBS
  antares-solver-variable
  result_writer)
