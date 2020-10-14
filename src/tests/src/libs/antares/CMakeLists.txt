set(src_libs_antares "${CMAKE_SOURCE_DIR}/libs/antares")


# Adding a library embedding all Antares sources useful to test class Matrix

set(SRC_MATRIX_LIB
	# For confort in IDE, but not necessary
	${src_libs_antares}/array/matrix.h
	${src_libs_antares}/array/matrix.hxx
	
	# Necessary cpp files
	${src_libs_antares}/jit/jit.cpp
	${src_libs_antares}/string-to-double.cpp
	logs/logs.cpp
	)

add_library(lib-matrix ${SRC_MATRIX_LIB})
target_include_directories(lib-matrix
								PRIVATE
									"${CMAKE_CURRENT_SOURCE_DIR}"
									"${CMAKE_CURRENT_SOURCE_DIR}/logs"
									"${CMAKE_SOURCE_DIR}/ext/yuni/src"
)


# Building tests on Matrix save operations
set(SRC_TEST_MATRIX_SAVE
	logs/logs.h
	array/fill-matrix.h
	array/matrix-bypass-load.h
	array/tests-matrix-save.h
	array/tests-matrix-save.cpp
	)

add_executable(tests-matrix-save ${SRC_TEST_MATRIX_SAVE})
target_include_directories(tests-matrix-save
								PRIVATE
									"${src_libs_antares}/array"
									"${CMAKE_CURRENT_SOURCE_DIR}/logs"
									"${CMAKE_CURRENT_SOURCE_DIR}/jit"
									"${CMAKE_SOURCE_DIR}/ext/yuni/src"
)

target_link_libraries(tests-matrix-save
						PRIVATE
							lib-matrix
							yuni-static-core
							Boost::unit_test_framework
)

add_test(NAME save-matrix COMMAND tests-matrix-save)


# Building tests on Matrix load operations
set(SRC_TEST_MATRIX_LOAD
	array/fill-matrix.h
	array/matrix-bypass-load.h
	array/tests-matrix-load.h
	array/tests-matrix-load.cpp
	)

add_executable(tests-matrix-load ${SRC_TEST_MATRIX_LOAD})
target_include_directories(tests-matrix-load
								PRIVATE
									"${src_libs_antares}/array"
									"${CMAKE_CURRENT_SOURCE_DIR}/logs"
									"${CMAKE_CURRENT_SOURCE_DIR}/jit"
									"${CMAKE_SOURCE_DIR}/ext/yuni/src"
)


target_link_libraries(tests-matrix-load
						PRIVATE
							lib-matrix
							yuni-static-core
							Boost::unit_test_framework
)

add_test(NAME load-matrix COMMAND tests-matrix-load)
