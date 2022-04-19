cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Make found targets globally available.
if (Boost_FOUND)
	set_target_properties(Boost::unit_test_framework PROPERTIES IMPORTED_GLOBAL TRUE)
endif()


add_subdirectory(src)

#end to end test require boost 1.6.x because for boost::test_tools::tolerance 
if(Boost_VERSION VERSION_GREATER 106000)
    add_subdirectory(end-to-end)
endif()

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)

  #check if pytest and numpy are installed
  find_python_module(pytest)
  find_python_module(numpy)

  if (PYTHON_MODULE_pytest_FOUND AND PYTHON_MODULE_numpy_FOUND)
  
      #Initialize test repository
      find_package(Git QUIET)

        set(ANTARES_TEST_TAG "v8.2")

        message(STATUS "Submodule test update with ${ANTARES_TEST_TAG} tag")

        # Update test submodules as needed
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init src/tests/resources/Antares_Simulator_Tests
                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
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
        
        # Checkout non-regression tests repo on current tag
        execute_process(COMMAND ${GIT_EXECUTABLE} checkout ${ANTARES_TEST_TAG}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/resources/NonRegTests
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git checkout ${ANTARES_TEST_TAG} failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
        
        macro(run_examples_tests use_ortools ortools_solver)
        
            if(${use_ortools})
                SET(SHORT_EXAMPLES_NAME short-examples-ortools-${ortools_solver})
                SET(MEDIUM_EXAMPLES_NAME medium-examples-ortools-${ortools_solver})
                SET(LONG_EXAMPLES_NAME long-examples-ortools-${ortools_solver})
                
                set(ADDITIONNAL_LABELS ortools ${ortools_solver})
                
                set(use_ortools_option "--use-ortools")
            else()
                SET(SHORT_EXAMPLES_NAME short-examples-sirius)
                SET(MEDIUM_EXAMPLES_NAME medium-examples-sirius)
                SET(LONG_EXAMPLES_NAME long-examples-sirius)
                                
                set(ADDITIONNAL_LABELS sirius)
                
                set(use_ortools_option "")
                
            endif()
        
            # Short tests
            add_test(
                NAME ${SHORT_EXAMPLES_NAME}
                COMMAND Python3::Interpreter -m pytest -m short --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> ${use_ortools_option} --ortools-solver=${ortools_solver} example_test.py
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
            )
            set_property(TEST ${SHORT_EXAMPLES_NAME} PROPERTY LABELS short-examples ${ADDITIONNAL_LABELS})

            # Medium tests
            add_test(
                NAME ${MEDIUM_EXAMPLES_NAME}
                COMMAND Python3::Interpreter -m pytest -m medium --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> ${use_ortools_option} --ortools-solver=${ortools_solver} example_test.py
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
            )
            set_property(TEST ${MEDIUM_EXAMPLES_NAME} PROPERTY LABELS medium-examples ${ADDITIONNAL_LABELS})

            # Long tests
            add_test(
                NAME ${LONG_EXAMPLES_NAME}
                COMMAND Python3::Interpreter -m pytest -m long --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> ${use_ortools_option} --ortools-solver=${ortools_solver} example_test.py
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
            )
            set_property(TEST ${LONG_EXAMPLES_NAME} PROPERTY LABELS long-examples ${ADDITIONNAL_LABELS})
        
        endmacro()
        
        macro(add_output_tests use_ortools ortools_solver)
        
            if(${use_ortools})
                SET(SHORT_NAME short-output-ortools-${ortools_solver})                
                set(ADDITIONNAL_LABELS ortools ${ortools_solver})
                
                set(use_ortools_option "--use-ortools")
            else()
                SET(SHORT_NAME short-output-sirius)                                
                set(ADDITIONNAL_LABELS sirius)
                
                set(use_ortools_option "")
                
            endif()
        
            # Short tests
            add_test(
                NAME ${SHORT_NAME}
                COMMAND Python3::Interpreter -m pytest -m short --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> ${use_ortools_option} --ortools-solver=${ortools_solver} output_compare_test.py --tb=line -v -s
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
            )
            set_property(TEST ${SHORT_NAME} PROPERTY LABELS short-output ${ADDITIONNAL_LABELS})            
        
        endmacro()
                
        run_examples_tests(false "sirius")
        run_examples_tests(true "sirius")
        run_examples_tests(true "coin")
        
        add_output_tests(false, "sirius")
        
        # Specific tests
        add_test(
            NAME short-specific
            COMMAND Python3::Interpreter -m pytest -m short --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> specific_test.py
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        )
        set_property(TEST short-specific PROPERTY LABELS short-specific sirius)
        
        add_test(
            NAME medium-specific
            COMMAND Python3::Interpreter -m pytest -m medium --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> specific_test.py
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        )
        set_property(TEST medium-specific PROPERTY LABELS medium-specific sirius)

        # Non-regression tests
        add_test(
            NAME non-regression
            COMMAND Python3::Interpreter -m pytest --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver> nonreg_compare_test.py --tb=line -v -s
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        )
        set_property(TEST non-regression PROPERTY LABELS non-regression sirius)
  else()   
      message(FATAL "Module pytest or numpy not installed : can't run python scripts for end to end tests" )
  endif()
  
endif()
