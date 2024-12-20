include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")
set(src_libs_antares_checks "${CMAKE_SOURCE_DIR}/libs/antares/checks")

# ====================================
# Tests on thermal price definition
# ====================================
add_boost_test(thermal-price-definition
  SRC thermal-price-definition.cpp
  LIBS
  checks
  Antares::study
  Antares::exception
  Antares::checks
  INCLUDE
  "${src_libs_antares_study}/parts/thermal"
  "${src_libs_antares_study}/area"
  "${src_libs_antares_study}"
  "${src_libs_antares_checks}")

# Linux
if(UNIX AND NOT APPLE)
target_link_libraries(thermal-price-definition PRIVATE stdc++fs)
endif()
