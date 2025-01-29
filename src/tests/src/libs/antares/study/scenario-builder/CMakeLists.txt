include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

# ====================================
# Tests on reading scenario-builder
# ====================================
add_boost_test(test-sc-builder-file-read-line
  SRC test-sc-builder-file-read-line.cpp
  INCLUDE "${src_libs_antares_study}/include"
  LIBS model_antares)

# ====================================
# Tests on saving scenario-builder
# ====================================
add_boost_test(test-sc-builder-file-save
  SRC
  test-sc-builder-file-save.cpp
  "${src_tests_src_libs_antares_study}/area/files-helper.cpp"
  INCLUDE
  "${src_libs_antares_study}"
  "${src_libs_antares_study}/scenario-builder"
  "${src_tests_src_libs_antares_study}"
  LIBS
  model_antares)
