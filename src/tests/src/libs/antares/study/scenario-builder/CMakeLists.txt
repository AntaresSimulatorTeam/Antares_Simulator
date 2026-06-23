include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/src/libs/antares/study")

# ====================================
# Tests on reading scenario-builder
# ====================================
add_boost_test(test-sc-builder-file-read-line
  SRC test-sc-builder-file-read-line.cpp
  INCLUDE "${src_libs_antares_study}/include"
  LIBS model_antares)


add_boost_test(test-sc-builder-get-set
  SRC test-sc-builder-get-set.cpp
  INCLUDE
  "${src_libs_antares_study}"
  "${src_libs_antares_study}/scenario-builder"
  "${src_tests_src_libs_antares_study}"
  LIBS
  model_antares)
