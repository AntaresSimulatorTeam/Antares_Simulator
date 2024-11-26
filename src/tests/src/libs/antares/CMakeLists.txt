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
	logs/logs.cpp
	)

add_library(matrix ${SRC_MATRIX_LIB})

target_link_libraries(matrix
		PUBLIC
		yuni-static-core
)

target_include_directories(matrix
								PRIVATE
									"${CMAKE_CURRENT_SOURCE_DIR}/logs"
									"${src_libs_antares}/jit/include"
		)

# Storing lib-matrix under the folder Unit-tests in the IDE
set_target_properties(matrix PROPERTIES FOLDER Unit-tests)

# Building tests on Matrix save operations
set(SRC_TEST_MATRIX_SAVE
	logs/antares/logs/logs.h
	array/fill-matrix.h
	array/matrix-bypass-load.h
	array/tests-matrix-save.h
	array/tests-matrix-save.cpp
	)

add_executable(tests-matrix-save ${SRC_TEST_MATRIX_SAVE})

target_include_directories(tests-matrix-save
								PRIVATE
									"${src_libs_antares}/array/include"
									"${src_libs_antares}/io/include"
									"${src_libs_antares}/jit/include"
									"${src_libs_antares}/memory/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/logs"
									"${CMAKE_CURRENT_SOURCE_DIR}/jit"
									"${CMAKE_SOURCE_DIR}/tests/src/libs"

		)

target_link_libraries(tests-matrix-save
						PRIVATE
							matrix
							yuni-static-core
							Boost::unit_test_framework
							antares-core
)

# Storing tests-matrix-save under the folder Unit-tests in the IDE
set_target_properties(tests-matrix-save PROPERTIES FOLDER Unit-tests)

add_test(NAME save-matrix COMMAND tests-matrix-save)

set_property(TEST save-matrix PROPERTY LABELS unit)

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
									"${src_libs_antares}/array/include"
									"${src_libs_antares}/io/include"
									"${src_libs_antares}/jit/include"
									"${src_libs_antares}/memory/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/logs"
									"${CMAKE_CURRENT_SOURCE_DIR}/jit"
									"${CMAKE_SOURCE_DIR}/tests/src/libs"
)


target_link_libraries(tests-matrix-load
						PRIVATE
							matrix
							yuni-static-core
							Boost::unit_test_framework
							antares-core
)

# Storing tests-matrix-load under the folder Unit-tests in the IDE
set_target_properties(tests-matrix-load PROPERTIES FOLDER Unit-tests)

add_test(NAME load-matrix COMMAND tests-matrix-load)

set_property(TEST load-matrix PROPERTY LABELS unit)


add_executable(test-utils test_utils.cpp)
target_link_libraries(test-utils
		PRIVATE
		Boost::unit_test_framework
		Antares::utils
		yuni-static-core
)
set_target_properties(test-utils PROPERTIES FOLDER Unit-tests/test-utils)

add_test(NAME test-utils COMMAND test-utils)
set_property(TEST test-utils PROPERTY LABELS unit)
