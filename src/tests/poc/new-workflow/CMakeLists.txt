add_executable(simple-test simple-test.cpp
)
target_link_libraries(simple-test
        Antares::optim_api
        Antares::optim_impl
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_test(NAME simple-test COMMAND simple-test)
set_property(TEST simple-test PROPERTY LABELS poc)



add_executable(standard-test standard-test.cpp
)
target_link_libraries(standard-test
        Antares::optim_api
        Antares::optim_impl
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME standard-test COMMAND standard-test)
set_property(TEST standard-test PROPERTY LABELS poc)


add_executable(scaling-test scaling-test.cpp
)
target_link_libraries(scaling-test
        Antares::optim_api
        Antares::optim_impl
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME scaling-test COMMAND scaling-test)
set_property(TEST scaling-test PROPERTY LABELS poc)
