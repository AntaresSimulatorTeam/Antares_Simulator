include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)
add_boost_test(test-system-model
  SRC
  test_main.cpp
  test_component.cpp
  test_system.cpp
  test_library.cpp
  LIBS
  Antares::exception
  Antares::expressions
  Antares::model-converter
  Antares::yml-model
  Antares::yml-system
  Antares::antares-study-system-model
  Antares::antlr-interface
  Antares::load-modeler-files
  Antares::modeler-lib # TODO dependency of Antares::load-modeler-files
  test_utils_unit)
