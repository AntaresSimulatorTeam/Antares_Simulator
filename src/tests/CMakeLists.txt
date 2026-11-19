cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

find_package(Boost CONFIG COMPONENTS unit_test_framework REQUIRED)

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
    add_subdirectory(inmemory-study)
    add_subdirectory(inmemory-modeler)
    add_subdirectory(libs)
    add_subdirectory(solver)
else()
    message(STATUS "Boost >= 1.60.0 is required for end-to-end tests, found ${Boost_VERSION}. Skipping")
endif()

add_subdirectory(run-study-tests)
