include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

add_boost_test(test-folder-output
  SRC study.cpp
  INCLUDE "${src_libs_antares_study}"
  LIBS Antares::study)
