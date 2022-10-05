# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

# ===================================
# Tests on area's optimization.ini
# ===================================
set(SRC_STUDY_LOADER
	# files-generator.h
	# files-generator.cpp
	test-study-load-internal.cpp
)
add_executable(test-study-load-internal ${SRC_STUDY_LOADER})

target_include_directories(test-study-load-internal
								PRIVATE
									"${src_libs_antares_study}"
)
target_link_libraries(test-study-load-internal
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
)

# Storing test-study-load-internal under the folder Unit-tests in the IDE
set_target_properties(test-study-load-internal PROPERTIES FOLDER Unit-tests)

add_test(NAME study-load-internal COMMAND test-study-load-internal)

set_property(TEST study-load-internal PROPERTY LABELS unit)

