include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(TestDataSeriesCsvImporter
  SRC
  test_main.cpp
  testDataSeriesCsvImporter.cpp
  LIBS
  Antares::data-series-csv-importer
  test_utils_unit)
