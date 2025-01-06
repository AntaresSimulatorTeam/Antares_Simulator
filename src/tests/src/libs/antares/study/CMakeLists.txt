include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

set(src_tests_src_libs_antares_study "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(area)
add_subdirectory(scenario-builder)
add_subdirectory(output-folder)
add_subdirectory(short-term-storage-input)
add_subdirectory(thermal-price-definition)
add_subdirectory(constraint)
add_subdirectory(parts)
add_subdirectory(series)
add_subdirectory(parameters)

add_boost_test(test-study
  SRC test_study.cpp
  LIBS Antares::study)
