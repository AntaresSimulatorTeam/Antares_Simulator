include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# ====================================
# Tests on Reserves Functions
# ====================================
add_boost_test(test_study_reserves_load
  SRC test_study_reserves_load.cpp
  LIBS Antares::study
  test_utils_unit)

add_boost_test(test_study_reserves_import_pb_hebdo
  SRC test_study_reserves_import_pb_hebdo.cpp
  LIBS Antares::study
  test_utils_unit)

add_boost_test(test_symmetries
  SRC test_symmetries.cpp
  LIBS Antares::study
  test_utils_unit)
