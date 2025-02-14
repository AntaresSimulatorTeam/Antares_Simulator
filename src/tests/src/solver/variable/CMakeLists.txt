include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-intermediate
  SRC test_intermediate.cpp
  LIBS antares-solver-variable)
