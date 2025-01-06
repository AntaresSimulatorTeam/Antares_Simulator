include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test_inifile_io
  SRC test_inifile_io.cpp
  LIBS inifile)
