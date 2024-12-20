include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(short-term-storage-input
  SRC short-term-storage-input-output.cpp
  LIBS model_antares
  INCLUDE "${src_libs_antares_study}/parts/short-term-storage")

# Linux
if(UNIX AND NOT APPLE)
  target_link_libraries(short-term-storage-input PRIVATE stdc++fs)
endif()
