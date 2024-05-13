# ====================================
# Tests on Parameters class
# ====================================
set(SRC_PARAMETERS_TESTS
	parameters-tests.cpp
)
add_executable(parameters-tests ${SRC_PARAMETERS_TESTS})

target_link_libraries(parameters-tests
                      PRIVATE
                      Boost::unit_test_framework
                      Antares::study
)

# Storing parameters-tests under the folder Unit-tests in the IDE
set_target_properties(parameters-tests PROPERTIES FOLDER Unit-tests/parameters-tests)

add_test(NAME parameters-tests COMMAND parameters-tests)

set_property(TEST parameters-tests PROPERTY LABELS unit)
