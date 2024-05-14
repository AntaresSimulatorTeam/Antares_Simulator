
set(src_libs_antares "${CMAKE_SOURCE_DIR}/libs/antares")

add_executable(test_inifile_io	test_inifile_io.cpp)


target_include_directories(test_inifile_io
						     PRIVATE
								"${src_libs_antares}/inifile/include/antares/inifile"
)


target_link_libraries(test_inifile_io
						PRIVATE
							Boost::unit_test_framework
							model_antares
)


# Storing executable under the folder Unit-tests in the IDE
set_target_properties(test_inifile_io PROPERTIES FOLDER Unit-tests/test_inifile_io)

add_test(NAME test_inifile_io COMMAND test_inifile_io)

set_property(TEST test_inifile_io PROPERTY LABELS unit)