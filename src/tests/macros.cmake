# The following function allows to add a test in a single line
# Arguments
# SRC path to the sources
# (optional) LIBS path to the libs to link
# (optional) INCLUDE include paths
# NOTE it's not necessary to add Boost::unit_test_framework

function(add_boost_test)
    set(options "")
    set(oneValueArgs)
    set(multiValueArgs SRC LIBS INCLUDE)
    cmake_parse_arguments(PARSE_ARGV 0 arg
        "${options}" "${oneValueArgs}" "${multiValueArgs}")
    # Bypass cmake_parse_arguments for the 1st argument
    set(TEST_NAME ${ARGV0})
    add_executable(${TEST_NAME} ${arg_SRC})
    # All tests use boost
    target_link_libraries(${TEST_NAME} PRIVATE ${arg_LIBS} Boost::unit_test_framework)

    # Optional: add private include directories
    if (NOT "${arg_INCLUDE}" STREQUAL "")
      target_include_directories(${TEST_NAME} PRIVATE ${arg_INCLUDE})
    endif()

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # Adding labels allows ctest filter what tests to run
    set_property(TEST ${TEST_NAME} PROPERTY LABELS unit)

    # Give the IDE some directions to display tests in a "Unit-tests" folder
    set_target_properties(${TEST_NAME} PROPERTIES FOLDER Unit-tests)

    # Linux only. TODO remove ?
    if(UNIX AND NOT APPLE)
      target_link_libraries(${TEST_NAME} PRIVATE stdc++fs)
    endif()

endfunction()
