include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-lps
  SRC test_lps.cpp
  LIBS antares-solver-simulation)
