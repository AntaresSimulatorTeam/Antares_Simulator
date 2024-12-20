include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test_constraint
  SRC test_constraint.cpp
  LIBS
  test_utils_unit
  Antares::study)

if(UNIX AND NOT APPLE)
    target_link_libraries(test_constraint PRIVATE stdc++fs)
endif()

add_boost_test(test_groups
  SRC test_group.cpp
  LIBS
  test_utils_unit
  Antares::study)

if(UNIX AND NOT APPLE)
    target_link_libraries(test_groups PRIVATE stdc++fs)
endif()
