# Useful variables definitions
set(CMAKE_CXX_STANDARD 17)
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

add_executable(test-save-link-properties test-save-link-properties.cpp)

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

