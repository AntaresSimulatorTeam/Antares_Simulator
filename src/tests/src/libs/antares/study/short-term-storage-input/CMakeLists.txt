include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

add_boost_test(short-term-storage-input
  SRC short-term-storage-input-output.cpp
  LIBS model_antares
        additional-constraint-rhs-antlr-interface
  INCLUDE "${src_libs_antares_study}/parts/short-term-storage")

add_boost_test(test-make-hour-groups-from-string
  SRC test-on-making-hour-groups-from-string.cpp
      ${src_libs_antares_study}/parts/short-term-storage/makeGroupsOfHoursFromString.cpp
        LIBS additional-constraint-rhs-antlr-interface
  INCLUDE "${src_libs_antares_study}/include")