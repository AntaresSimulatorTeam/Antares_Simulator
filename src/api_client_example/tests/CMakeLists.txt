find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(test_client)

target_sources(test_client
        PRIVATE
        test.cpp
)

target_link_libraries(test_client
        PRIVATE
        API_client
        Boost::unit_test_framework
)

add_test(NAME testclient COMMAND test_client)
set_property(TEST testclient PROPERTY LABELS integ)