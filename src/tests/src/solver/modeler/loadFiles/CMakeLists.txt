include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(loadFiles
  SRC
  testLoadModelerFiles.cpp
  testParameters.cpp
  LIBS
  Antares::modelerParameters
        Antares::loadModelerFiles
  test_utils_unit)
