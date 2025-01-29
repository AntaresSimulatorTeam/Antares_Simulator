include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(TestModelParser
  SRC
  testModelParser.cpp
  testModelTranslator.cpp
  testConvertorVisitor.cpp
  test_full.cpp
  enum_operators.h
  testSystemParser.cpp
  testSystemConverter.cpp
  LIBS
  Antares::solver-expressions
  Antares::modelConverter
  Antares::modelParser
  Antares::systemParser
  Antares::antares-study-system-model
  Antares::antlr-interface)
