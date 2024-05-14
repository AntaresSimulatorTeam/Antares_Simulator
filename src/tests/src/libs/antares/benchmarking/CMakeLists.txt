set(PROJ test-duration-collector)

add_executable(${PROJ})
target_sources(${PROJ} PRIVATE test_duration_collector.cpp)
target_link_libraries(${PROJ}
        PRIVATE
        Antares::benchmarking
        Boost::boost Boost::disable_autolinking
)

add_test(NAME ${PROJ} COMMAND ${PROJ})

set_property(TEST ${PROJ} PROPERTY LABELS unit)
