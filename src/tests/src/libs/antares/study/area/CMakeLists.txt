# Useful variables definitions
set(CMAKE_CXX_STANDARD 17)
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

add_executable(test-save-area-optimization-ini test-save-area-optimization-ini.cpp)

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

