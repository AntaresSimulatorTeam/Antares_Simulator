set(PROJ test-duration-collector)

add_executable(${PROJ})
target_sources(${PROJ} PRIVATE test_duration_collector.cpp)
target_link_libraries(${PROJ}
        PRIVATE
        Antares::benchmarking
        Boost::unit_test_framework
)

set_target_properties(${PROJ} PROPERTIES FOLDER Unit-tests/${PROJ})

add_test(NAME ${PROJ} COMMAND ${PROJ})

set_property(TEST ${PROJ} PROPERTY LABELS unit)
