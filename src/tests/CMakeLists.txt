cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Make found targets globally available.
if (Boost_FOUND)
    set_target_properties(Boost::unit_test_framework PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

add_subdirectory(cucumber)

# end to end test require boost 1.6.x because of boost::test_tools::tolerance, BOOST_TEST
# old versions of Boost don't contain a '.', also ignore them
# example : Boost 1.53.0 returns 105300 as Boost_VERSION
string(FIND ${Boost_VERSION} . RESULT_BOOST)
if (${RESULT_BOOST} EQUAL -1) # e.g 105300
  if (${Boost_VERSION} GREATER_EQUAL 106000)
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
    add_subdirectory(src)
    add_subdirectory(kirchhoff-cbuilder)
    add_subdirectory(inmemory-study)
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
        COMMAND Python3::Interpreter -m pytest -m unfeasible --antares-simu-path=$<TARGET_FILE:antares-solver>
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

      add_test(
        NAME json
        COMMAND Python3::Interpreter -m pytest -m json --antares-simu-path=$<TARGET_FILE:antares-solver>
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

      add_test(
        NAME named-mps
        COMMAND Python3::Interpreter -m pytest -m json --antares-simu-path=$<TARGET_FILE:antares-solver> --named-mps-problems
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

      add_test(
        NAME milp-cbc
        COMMAND Python3::Interpreter -m pytest -m json --antares-simu-path=$<TARGET_FILE:antares-solver> --solver coin
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

      add_test(
        NAME parallel
        COMMAND Python3::Interpreter -m pytest -m json --antares-simu-path=$<TARGET_FILE:antares-solver> --force-parallel
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

# The Kirchhoff constraint builder is built only if BUILD_TOOLS=ON
if (BUILD_TOOLS)
      add_test(
        NAME kirchhoff
        COMMAND Python3::Interpreter -m pytest -m kirchhoff --exe_kirchhoff_path=$<TARGET_FILE:antares-kirchhoff-constraints-builder> kirchhoff_pytest.py
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/kirchhoff-cbuilder"
        )

      add_test(
        NAME tsgenerator
        COMMAND Python3::Interpreter -m pytest -m tsgenerator --tsgenerator_path=$<TARGET_FILE:antares-ts-generator> antares_tools/ts-generator/ts-generator.py
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/run-study-tests"
        )

endif()

      set_property(TEST unfeasible PROPERTY LABELS study sirius)
  else()
      message(FATAL_ERROR "Module pytest or numpy not installed : can't run python scripts for end to end tests")
  endif()

endif()
