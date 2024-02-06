find_package(Boost COMPONENTS unit_test_framework REQUIRED)
enable_testing()

#bigobj support needed for windows compilation
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

add_executable(tests-simple-test
		simple-test.cpp
		)

target_link_libraries(tests-simple-test
						PRIVATE
						test_utils
						optim_api
						optim_impl
						${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			)
			
target_include_directories(tests-simple-test
							PRIVATE
							${CMAKE_SOURCE_DIR}/optim
							)

add_test(NAME end-to-end-simple-test COMMAND tests-simple-test)
#set_property(TEST end-to-end-simple-test PROPERTY LABELS end-to-end)
#set_target_properties(tests-simple-test PROPERTIES FOLDER Unit-tests/end_to_end)

# Storing tests-simple-study under the folder Unit-tests in the IDE

#----------------------------------------------------------
