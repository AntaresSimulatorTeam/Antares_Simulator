add_executable(simple-test simple-test.cpp
)
target_link_libraries(simple-test
        Antares::optim_api
        Antares::optim_impl
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(standard-test standard-test.cpp
)
target_link_libraries(standard-test
        Antares::optim_api
        Antares::optim_impl
        Antares::optim_standard_api
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(scaling-test scaling-test.cpp
)
target_link_libraries(scaling-test
        Antares::optim_api
        Antares::optim_impl
        Antares::optim_standard_api
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})