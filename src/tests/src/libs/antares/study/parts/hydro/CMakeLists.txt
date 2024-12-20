include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# Hydro data reader
add_boost_test(test-hydro-reader
  SRC test-hydroreader-class.cpp
  LIBS
  Antares::study
  test_utils_unit)

# Hydro series
add_boost_test(test-hydro-series
  SRC test-hydro-series.cpp
  LIBS
  Antares::study
  test_utils_unit)
