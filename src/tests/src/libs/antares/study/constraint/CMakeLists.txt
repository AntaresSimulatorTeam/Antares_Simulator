add_executable(test_constraint test_constraint.cpp)

target_link_libraries(test_constraint
        PRIVATE
        libantares-core
        Boost::unit_test_framework
        )

if(UNIX AND NOT APPLE)
    target_link_libraries(test_constraint PRIVATE stdc++fs)
endif()

add_test(NAME test_constraint COMMAND test_constraint)

set_property(TEST test_constraint PROPERTY LABELS unit)