include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(parse-parameters
  SRC testParametersParsing.cpp
  LIBS
  modeler-parameters
  test_utils_unit)
