include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-client-api
  SRC test_api.cpp
  LIBS Antares::solver_api)
