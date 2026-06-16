include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(test-lps
  SRC test_lps.cpp
  LIBS solver-lib)
