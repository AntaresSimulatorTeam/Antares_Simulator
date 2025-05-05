include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# ====================================
# Tests on TimeSeries class
# ====================================
add_boost_test(timeseries-tests
  SRC timeseries-tests.cpp
  LIBS
  Antares::array
  Antares::series
  antares-solver-simulation)
