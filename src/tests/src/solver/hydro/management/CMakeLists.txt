include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

set(src_solver_hydro "${CMAKE_SOURCE_DIR}/solver/hydro")

# Hydro input checker
add_boost_test(test-hydro-inputs-checker
  SRC test-hydro-inputs-checker.cpp
  LIBS
  Antares::study
  antares-solver-hydro
  test_utils_unit)

add_boost_test(unit-tests-hydro
  SRC
  monthly.cpp
  INCLUDE
  "${src_solver_hydro}"
  LIBS
  antares-solver-simulation)




