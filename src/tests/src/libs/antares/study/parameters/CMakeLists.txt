include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

# ====================================
# Tests on Parameters class
# ====================================
add_boost_test(parameters-tests
  SRC parameters-tests.cpp
  LIBS Antares::study
  antares-solver-variable)
