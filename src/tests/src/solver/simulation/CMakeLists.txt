include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# Useful variables definitions
set(src_solver_simulation "${CMAKE_SOURCE_DIR}/solver/simulation")
set(src_solver_hydro "${CMAKE_SOURCE_DIR}/solver/hydro")
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

set(SRC_TS_NUMBERS
	# For confort in IDE, but not necessary
	${src_solver_simulation}/include/antares/solver/simulation/timeseries-numbers.h
	
	# Necessary cpp files
	${src_solver_simulation}/timeseries-numbers.cpp
	${src_solver_simulation}/include/antares/solver/simulation/ITimeSeriesNumbersWriter.h)

add_boost_test(tests-ts-numbers
  SRC tests-ts-numbers.cpp ${SRC_TS_NUMBERS}
  INCLUDE
  "${src_solver_simulation}"
  "${src_libs_antares_study}"
  LIBS
  Antares::utils
  model_antares
  antares-solver-simulation
  antares-solver-ts-generator)


# ===================================
# Tests on area's store-timeseries-number
# ===================================
add_boost_test(test-store-timeseries-number
  SRC test-store-timeseries-number.cpp
  LIBS
  test_utils_unit
  antares-solver-simulation
  Antares::study
  Antares::result_writer)

# ===================================
# Tests on time series
# ===================================
add_boost_test(test-time_series
  SRC test-time_series.cpp
  LIBS
  test_utils_unit
  antares-solver-simulation
  Antares::study)

# ===================================
# Tests on hydro final reservoir level functions
# ===================================
add_boost_test(test-hydro_final
  SRC
  test-hydro-final-reservoir-level-functions.cpp
  INCLUDE
  "${src_solver_simulation}"
  "${src_libs_antares_study}"
  "${src_solver_hydro}"
  LIBS
  Antares::study
  antares-solver-simulation
  Antares::array)
