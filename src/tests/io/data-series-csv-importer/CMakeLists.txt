include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(TestDataSeriesCsvImporter
  SRC
  testDataSeriesRepoImporter.cpp
  LIBS
  Antares::data-series-csv-importer)
