include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

# ================================
# Tests on link's properties.ini
# ================================
set(SRC_LINK_PROPERTIES
	files-helper.h
	files-helper.cpp
	test-save-link-properties.cpp
)
add_boost_test(test-save-link-properties
  SRC ${SRC_LINK_PROPERTIES}
  INCLUDE "${src_libs_antares_study}/include"
  LIBS model_antares)

# ===================================
# Tests on area's optimization.ini
# ===================================
set(SRC_AREA_OPTIMIZATION
	files-helper.h
	files-helper.cpp
	test-save-area-optimization-ini.cpp)

add_boost_test(test-save-area-optimization-ini
  SRC ${SRC_AREA_OPTIMIZATION}
  INCLUDE "${src_libs_antares_study}/include"
  LIBS model_antares)
