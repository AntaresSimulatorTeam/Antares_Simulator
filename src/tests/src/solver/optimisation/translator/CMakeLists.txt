include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(test-translator
  SRC test_translator.cpp
  LIBS model_antares)
