cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Make found targets globally available.
if (Boost_FOUND)
	set_target_properties(Boost::unit_test_framework PROPERTIES IMPORTED_GLOBAL TRUE)
endif()


add_subdirectory(src)

# end to end test require boost 1.6.x because of boost::test_tools::tolerance, BOOST_TEST
# old versions of Boost don't contain a '.', also ignore them
# example : Boost 1.53.0 returns 105300 as Boost_VERSION
string(FIND ${Boost_VERSION} . RESULT_BOOST)
if (${RESULT_BOOST} EQUAL -1) # e.g 105300
  if (GREATER_EQUAL ${Boost_Version} 106000)
    set(RECENT_BOOST 1)
  else()
    set(RECENT_BOOST 0)
  endif()
else() # e.g 1.60.00
  if (${Boost_VERSION} VERSION_GREATER_EQUAL "1.60.0")
    set(RECENT_BOOST 1)
  else()
    set(RECENT_BOOST 0)
  endif()
endif()

if (${RECENT_BOOST})
    add_subdirectory(end-to-end)
else()
    message(STATUS "Boost >= 1.60.0 is required for end-to-end tests, found ${Boost_VERSION}. Skipping")
endif()

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)

  #check if pytest and numpy are installed
  find_python_module(pytest)
  find_python_module(numpy)

  if (PYTHON_MODULE_pytest_FOUND AND PYTHON_MODULE_numpy_FOUND)
      # TODO : add more study batches
      add_test(
        NAME unfeasible
        COMMAND Python3::Interpreter -m pytest -m unfeasible --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver>
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

      add_test(
        NAME json
        COMMAND Python3::Interpreter -m pytest -n 2 -m json --solver-path=$<TARGET_FILE:antares-${ANTARES_PRG_VERSION}-solver>
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

      set_property(TEST unfeasible PROPERTY LABELS study sirius)
  else()   
      message(FATAL "Module pytest or numpy not installed : can't run python scripts for end to end tests")
  endif()
  
endif()
