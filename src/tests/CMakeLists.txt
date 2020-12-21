cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

#copy test data to build directory
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data/" 		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/data/")

find_package(Boost COMPONENTS unit_test_framework filesystem regex REQUIRED)

# Make found targets globally available.
if (Boost_FOUND)
	set_target_properties(Boost::unit_test_framework PROPERTIES IMPORTED_GLOBAL TRUE)
	set_target_properties(Boost::filesystem PROPERTIES IMPORTED_GLOBAL TRUE)
	set_target_properties(Boost::regex PROPERTIES IMPORTED_GLOBAL TRUE)
endif()


add_subdirectory(src)
add_subdirectory(end-to-end)


OMESSAGE("antares-example-tests")

find_package(Python3 EXACT 3.8 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  # Short tests
  add_test(
	NAME short-examples
	COMMAND Python3::Interpreter -m pytest -m short example_test.py
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
	)
  set_property(TEST short-examples PROPERTY LABELS TESTLABEL short)

  # Medium tests
  add_test(
	NAME medium-examples
	COMMAND Python3::Interpreter -m pytest -m medium example_test.py
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
	)
  set_property(TEST medium-examples PROPERTY LABELS TESTLABEL medium)

  # Long tests
  add_test(
	NAME long-examples
	COMMAND Python3::Interpreter -m pytest -m long example_test.py
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
	)
  set_property(TEST long-examples PROPERTY LABELS TESTLABEL long)
endif()
