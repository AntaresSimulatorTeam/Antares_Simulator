include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-client-api
  SRC test_api.cpp
  LIBS Antares::solver_api)

add_boost_test(test-single-problem-api
  SRC
  test_single_problem_api.cpp
  LIBS
  single_problem_api
  Antares::tests::in-memory-study
# While this is ugly, but we need Implementation::SingleProblemGetter
# since unlike SingleProblemGetter, it exposes the
# "Study" constructor, allowing to create the study in memory
  INCLUDE
  "${CMAKE_SOURCE_DIR}/api/private")
