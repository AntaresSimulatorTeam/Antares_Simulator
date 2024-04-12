add_executable(standard-test standard-test.cpp)
target_link_libraries(standard-test
        Antares::optim_api
        Antares::optim_impl
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(scaling-test scaling-test.cpp)
target_link_libraries(scaling-test
        Antares::optim_api
        Antares::optim_impl
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
