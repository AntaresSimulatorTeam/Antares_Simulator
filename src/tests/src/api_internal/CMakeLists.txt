include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-api
  SRC test_api.cpp
  LIBS
  Antares::solver_api
  Antares::tests::in-memory-study
  test_utils_unit
  #Allow to use the private headers
  INCLUDE
  $<TARGET_PROPERTY:Antares::solver_api,INCLUDE_DIRECTORIES>)

add_boost_test(test-api-modelerproblems
  SRC test-modelerProblems.cpp
  LIBS
  Antares::single_problem_api
  Antares::modeler-lib
  Antares::load-modeler-files
  Antares::tests::inmemory-modeler
  INCLUDE
  "${CMAKE_SOURCE_DIR}/api/private")
