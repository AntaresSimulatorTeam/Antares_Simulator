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

option(BUILD_OUTPUT_TEST "Activates output values comparison test building" OFF)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)

  #check if pytest and numpy are installed
  find_python_module(pytest)
  find_python_module(numpy)

  if (PYTHON_MODULE_pytest_FOUND AND PYTHON_MODULE_numpy_FOUND)
      # TODO : add more study batches
      add_test(
        NAME short
        COMMAND Python3::Interpreter -m pytest -m short --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver>
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

      set_property(TEST short PROPERTY LABELS study sirius)
  else()   
      message(FATAL "Module pytest or numpy not installed : can't run python scripts for end to end tests")
  endif()
  
endif()
