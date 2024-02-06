add_executable(simple-test simple-test.cpp
)

target_link_libraries(simple-test
        Antares::optim_api
        Antares::optim_impl
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})