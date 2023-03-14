find_package(Boost COMPONENTS unit_test_framework REQUIRED)
enable_testing()

add_executable(test_constraint test_constraint.cpp)

target_link_libraries(test_constraint
        PRIVATE
        libantares-core
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        )
target_compile_definitions(test_constraint PRIVATE "BOOST_TEST_DYN_LINK=1")

if(UNIX AND NOT APPLE)
    target_link_libraries(test_constraint PRIVATE stdc++fs)
endif()

add_test(NAME test_constraint COMMAND test_constraint)

set_property(TEST test_constraint PROPERTY LABELS unit)

file(COPY bindingconstraints.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY dummy_id.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
