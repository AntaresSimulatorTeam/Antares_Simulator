include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-basis-status
  SRC
  basis_status.cpp
  INCLUDE
  "${CMAKE_SOURCE_DIR}/solver/utils"
  LIBS
  ortools::ortools
  Antares::solverUtils)
