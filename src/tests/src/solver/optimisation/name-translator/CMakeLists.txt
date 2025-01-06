include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-name-translator
  SRC test_name_translator.cpp
  LIBS model_antares)
