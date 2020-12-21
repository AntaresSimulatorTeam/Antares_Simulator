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

  #check if pytest and numpy are installed
  find_python_module(pytest)
  find_python_module(numpy)

  if (PYTHON_MODULE_pytest_FOUND AND PYTHON_MODULE_numpy_FOUND)
  
      #Initialize test repository
      find_package(Git QUIET)

        set(ANTARES_TEST_TAG "reference")

        message(STATUS "Submodule test update with ${ANTARES_TEST_TAG} tag")

        # Update test submodules as needed
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init 
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/resources/
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()

        # Define submodule tag
        execute_process(COMMAND ${GIT_EXECUTABLE} checkout ${ANTARES_TEST_TAG}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/resources/Antares_Simulator_Tests
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git checkout ${ANTARES_TEST_TAG} failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
  
        # Short tests
        add_test(
            NAME short-examples
            COMMAND Python3::Interpreter -m pytest -m short --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> example_test.py
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        )
        set_property(TEST short-examples PROPERTY LABELS TESTLABEL short)

        # Medium tests
        add_test(
            NAME medium-examples
            COMMAND Python3::Interpreter -m pytest -m medium --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> example_test.py
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        )
        set_property(TEST medium-examples PROPERTY LABELS TESTLABEL medium)

        # Long tests
        add_test(
            NAME long-examples
            COMMAND Python3::Interpreter -m pytest -m long --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> example_test.py
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        )
        set_property(TEST long-examples PROPERTY LABELS TESTLABEL long)
  else()   
      message(FATAL "Module pytest or numpy not installed : can't run python scripts for end to end tests" )
  endif()
  
endif()
