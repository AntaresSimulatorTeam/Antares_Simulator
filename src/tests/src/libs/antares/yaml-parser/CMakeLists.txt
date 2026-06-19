include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(yaml-parser-test
  SRC test_yaml_parser.cpp
  LIBS yaml-cpp::yaml-cpp)
