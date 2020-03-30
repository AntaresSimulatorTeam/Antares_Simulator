set(src_libs_antares "${ANTARES_SRC}/libs/antares")


message("lis/antares/CMakeLists.txt - CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}")
message("lis/antares/CMakeLists.txt - src_libs_antares : ${src_libs_antares}")

# Adding a library embedding all Antares sources useful to test class Matrix
set(SRC_MATRIX_LIB
	${src_libs_antares}/array/matrix.h
	${src_libs_antares}/array/matrix.hxx
	${src_libs_antares}/array/matrix-to-buffer.h
	${src_libs_antares}/array/matrix-to-buffer.hxx
	${src_libs_antares}/jit/jit.cpp
	${src_libs_antares}/logs/logs.cpp
	${src_libs_antares}/io/statistics.cpp
	${src_libs_antares}/string-to-double.cpp
	)

add_library(matrix ${SRC_MATRIX_LIB})
set_target_properties(matrix PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${UNIT_LIBRARIES_DIR}")


# Building tests on class Matrix
set(SRC_TEST_MATRIX
	${src_libs_antares}/array/matrix.h
	${src_libs_antares}/array/matrix.hxx
	array/tests-matrix.h
	array/tests-matrix.cpp
	)
	
include_directories("${src_libs_antares}/array")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/array")
add_executable(tests-on-Matrix ${SRC_TEST_MATRIX})
target_link_libraries(tests-on-Matrix matrix ${YUNI_STATIC_CORE_LIB})