include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

# ====================================
# Tests on Reserves Functions
# ====================================
add_boost_test(test_study_rampes
  SRC test_study_rampes.cpp
  LIBS Antares::study
  test_utils_unit)