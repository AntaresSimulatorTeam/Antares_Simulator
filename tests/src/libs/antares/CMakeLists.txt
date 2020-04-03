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
	${src_libs_antares}/io/statistics.cpp
	${src_libs_antares}/string-to-double.cpp
	logs/fake-logs.h
	logs/fake-logs.cpp
	)

add_library(lib-matrix ${SRC_MATRIX_LIB})
set_target_properties(lib-matrix PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${UNIT_LIBRARIES_DIR}")
target_include_directories(lib-matrix PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(lib-matrix PRIVATE "${ANTARES_SRC}/ext/yuni/src")

# Building tests on Matrix save operations
set(SRC_TEST_MATRIX_SAVE
	${src_libs_antares}/array/matrix.h
	${src_libs_antares}/array/matrix.hxx
	array/fill-matrix.h
	array/matrix-bypass-load.h
	array/tests-matrix-save.h
	array/tests-matrix-save.cpp
	)
	

add_executable(tests-matrix-save ${SRC_TEST_MATRIX_SAVE})
target_include_directories(tests-matrix-save PRIVATE "${src_libs_antares}/array")
target_include_directories(tests-matrix-save PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(tests-matrix-save PRIVATE "${BOOST_PATH}")
target_include_directories(tests-matrix-save PRIVATE "${ANTARES_SRC}/ext/yuni/src")
target_link_libraries(tests-matrix-save lib-matrix ${YUNI_STATIC_CORE_LIB})


# Building tests on Matrix load operations
set(SRC_TEST_MATRIX_LOAD
	${src_libs_antares}/array/matrix.h
	${src_libs_antares}/array/matrix.hxx
	array/fill-matrix.h
	array/matrix-bypass-load.h
	array/tests-matrix-load.h
	array/tests-matrix-load.cpp
	)
	

add_executable(tests-matrix-load ${SRC_TEST_MATRIX_LOAD})
target_include_directories(tests-matrix-load PRIVATE "${src_libs_antares}/array")
target_include_directories(tests-matrix-load PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(tests-matrix-load PRIVATE "${BOOST_PATH}")
target_include_directories(tests-matrix-load PRIVATE "${ANTARES_SRC}/ext/yuni/src")
target_link_libraries(tests-matrix-load lib-matrix ${YUNI_STATIC_CORE_LIB})


