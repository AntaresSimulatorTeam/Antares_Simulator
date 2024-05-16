set(PROJ test-duration-collector)

add_executable(${PROJ})
add_compile_definitions(${PROJ} BOOST_TEST_DYN_LINK)
target_sources(${PROJ} PRIVATE test_duration_collector.cpp)
target_link_libraries(${PROJ}
        PRIVATE
        Antares::benchmarking
        Boost::unit_test_framework
)

add_test(NAME ${PROJ} COMMAND ${PROJ})

set_property(TEST ${PROJ} PROPERTY LABELS unit)
