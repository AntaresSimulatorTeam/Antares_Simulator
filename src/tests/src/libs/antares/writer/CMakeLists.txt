# Zip writer
add_executable(test-writer test_writer.cpp)

target_link_libraries(test-writer
        PRIVATE
        Boost::unit_test_framework
        Antares::result_writer
        test_utils_unit
        MINIZIP::minizip
)

set_target_properties(test-writer PROPERTIES FOLDER Unit-tests/test-writer)

add_test(NAME writer COMMAND test-writer)
set_tests_properties(writer PROPERTIES LABELS unit)
