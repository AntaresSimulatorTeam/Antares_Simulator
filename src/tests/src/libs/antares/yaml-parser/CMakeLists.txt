include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(yaml-parser-test
  SRC test_yaml_parser.cpp
  LIBS yaml-cpp)
