include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(TestYmlModel
  SRC
  testYmlModel.cpp
  testModelTranslator.cpp
  testConvertorVisitor.cpp
  test_full.cpp
  enum_operators.h
  testYmlSystem.cpp
  testSystemConverter.cpp
  LIBS
  Antares::expressions
  Antares::model-converter
  Antares::yml-model
  Antares::yml-system
  Antares::antares-study-system-model
  Antares::antlr-interface)
