# Useful variables definitions
# set(src_solver_simulation "${CMAKE_SOURCE_DIR}/solver/simulation")
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")


set(SRC_AREA_LINKS_SAVE_TO_FOLDER
	# For confort in IDE, but not necessary
	${src_libs_antares_study}/area/area.h
	
	# Necessary cpp files
	${src_libs_antares_study}/area/links.cpp
)

add_executable(test-save-link-properties test-save-link-properties.cpp ${SRC_AREA_LINKS_SAVE_TO_FOLDER})


target_include_directories(test-save-link-properties
								PRIVATE
									"${src_libs_antares_study}/area"
)
target_link_libraries(test-save-link-properties
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(test-save-link-properties PROPERTIES FOLDER Unit-tests)

add_test(NAME save-link-properties COMMAND test-save-link-properties)

set_property(TEST save-link-properties PROPERTY LABELS TESTLABEL unit)

