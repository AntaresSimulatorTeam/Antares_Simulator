include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test_constraint
  SRC test_constraint.cpp
  LIBS
  test_utils_unit
  Antares::study)

add_boost_test(test_groups
  SRC test_group.cpp
  LIBS
  test_utils_unit
  Antares::study)
