add_subdirectory(concurrency)
add_subdirectory(writer)
add_subdirectory(study)
add_subdirectory(benchmarking)
add_subdirectory(inifile)

add_subdirectory(yaml-parser)
add_subdirectory(antlr4-interface)

set(src_libs_antares "${CMAKE_SOURCE_DIR}/libs/antares")

# Adding a library embedding all Antares sources useful to test class Matrix

set(SRC_MATRIX_LIB
	# For confort in IDE, but not necessary
	${src_libs_antares}/array/include/antares/array/matrix.h
	${src_libs_antares}/array/include/antares/array/matrix.hxx
	
	# Necessary cpp files
	${src_libs_antares}/jit/jit.cpp
	logs/logs.cpp)

add_library(matrix ${SRC_MATRIX_LIB})

target_link_libraries(matrix
		PUBLIC
		yuni-static-core)

target_include_directories(matrix
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/logs"
  "${src_libs_antares}/jit/include")

# Building tests on Matrix save operations
add_boost_test(tests-matrix-save
  SRC
  logs/antares/logs/logs.h
  array/fill-matrix.h
  array/matrix-bypass-load.h
  array/tests-matrix-save.h
  array/tests-matrix-save.cpp
  INCLUDE
  "${src_libs_antares}/array/include"
  "${src_libs_antares}/io/include"
  "${src_libs_antares}/jit/include"
  "${src_libs_antares}/memory/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/logs"
  "${CMAKE_CURRENT_SOURCE_DIR}/jit"
  "${CMAKE_SOURCE_DIR}/tests/src/libs"
  LIBS
  matrix
  yuni-static-core
  antares-core)

# Building tests on Matrix load operations
add_boost_test(tests-matrix-load
  SRC
  array/fill-matrix.h
  array/matrix-bypass-load.h
  array/tests-matrix-load.h
  array/tests-matrix-load.cpp
  INCLUDE
  "${src_libs_antares}/array/include"
  "${src_libs_antares}/io/include"
  "${src_libs_antares}/jit/include"
  "${src_libs_antares}/memory/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/logs"
  "${CMAKE_CURRENT_SOURCE_DIR}/jit"
  "${CMAKE_SOURCE_DIR}/tests/src/libs"
  LIBS
  matrix
  yuni-static-core
  antares-core)

# Test utilities
add_boost_test(test-utils
               SRC test_utils.cpp
	       LIBS
	       Antares::utils
	       yuni-static-core)
