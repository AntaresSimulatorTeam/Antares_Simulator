add_executable(test_constraint
        test_constraint.cpp
        test_group.cpp
    )

target_link_libraries(test_constraint
        PRIVATE
        libantares-core
        Boost::unit_test_framework
        )
		
# Storing test_constraint under the folder Unit-tests in the IDE
set_target_properties(test_constraint PROPERTIES FOLDER Unit-tests)

if(UNIX AND NOT APPLE)
    target_link_libraries(test_constraint PRIVATE stdc++fs)
endif()

add_test(NAME test_constraint COMMAND test_constraint)

set_property(TEST test_constraint PROPERTY LABELS unit)