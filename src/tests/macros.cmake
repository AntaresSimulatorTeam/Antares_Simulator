function(add_boost_test)
    set(options "")
    set(oneValueArgs)
    set(multiValueArgs SRC LIBS)
    cmake_parse_arguments(PARSE_ARGV 0 arg
        "${options}" "${oneValueArgs}" "${multiValueArgs}")
    # Bypass cmake_parse_arguments for the 1st argument
    set(TEST_NAME ${ARGV0})
    add_executable(${TEST_NAME} ${arg_SRC})
    # All tests use boost
    target_link_libraries(${TEST_NAME} PRIVATE ${arg_LIBS} Boost::unit_test_framework)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_property(TEST ${TEST_NAME} PROPERTY LABELS unit)
endfunction()
