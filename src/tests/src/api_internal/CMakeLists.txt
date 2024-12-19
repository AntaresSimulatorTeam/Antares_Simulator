include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-api
  SRC test_api.cpp
  LIBS
  Antares::solver_api
  Antares::tests::in-memory-study
  test_utils_unit)

target_include_directories(test-api
        PRIVATE
        #Allow to use the private headers
        $<TARGET_PROPERTY:Antares::solver_api,INCLUDE_DIRECTORIES>)
