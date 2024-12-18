# Zip writer
add_executable(parse-parameters testParametersParsing.cpp)

target_link_libraries(parse-parameters
        PRIVATE
        Boost::unit_test_framework
        modeler-parameters
        test_utils_unit
)

set_target_properties(parse-parameters PROPERTIES FOLDER Unit-tests/test-writer)

add_test(NAME parse-parameters COMMAND parse-parameters)
set_tests_properties(parse-parameters PROPERTIES LABELS unit)
