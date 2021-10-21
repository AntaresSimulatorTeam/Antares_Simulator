# Useful variables definitions
set(CMAKE_CXX_STANDARD 17)
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

# ================================
# Tests on link's properties.ini
# ================================
set(SRC_LINK_PROPERTIES
	files-helper.h
	files-helper.cpp
	test-save-link-properties.cpp
)
add_executable(test-save-link-properties ${SRC_LINK_PROPERTIES})

target_include_directories(test-save-link-properties
								PRIVATE
									"${src_libs_antares_study}"
)
target_link_libraries(test-save-link-properties
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
)

# Storing test-save-link-properties under the folder Unit-tests in the IDE
set_target_properties(test-save-link-properties PROPERTIES FOLDER Unit-tests)

add_test(NAME save-link-properties COMMAND test-save-link-properties)

set_property(TEST save-link-properties PROPERTY LABELS TESTLABEL unit)

# ===================================
# Tests on area's optimization.ini
# ===================================
set(SRC_AREA_OPTIMIZATION
	files-helper.h
	files-helper.cpp
	test-save-area-optimization-ini.cpp
)
add_executable(test-save-area-optimization-ini ${SRC_AREA_OPTIMIZATION})

target_include_directories(test-save-area-optimization-ini
								PRIVATE
									"${src_libs_antares_study}"
)
target_link_libraries(test-save-area-optimization-ini
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
)

# Storing test-save-area-optimization-ini under the folder Unit-tests in the IDE
set_target_properties(test-save-area-optimization-ini PROPERTIES FOLDER Unit-tests)

add_test(NAME save-area-optimization COMMAND test-save-area-optimization-ini)

set_property(TEST save-area-optimization PROPERTY LABELS TESTLABEL unit)

