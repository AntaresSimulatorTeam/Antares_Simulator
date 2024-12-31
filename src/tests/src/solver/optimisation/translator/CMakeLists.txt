include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-translator
  SRC test_translator.cpp
  LIBS model_antares)
