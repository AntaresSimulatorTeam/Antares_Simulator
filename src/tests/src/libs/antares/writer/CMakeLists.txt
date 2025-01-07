include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# Zip writer
add_boost_test(test-writer
  SRC test_writer.cpp
  LIBS
  Antares::result_writer
  test_utils_unit
  MINIZIP::minizip)

