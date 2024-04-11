add_executable(test-check-input-data)

target_sources(test-check-input-data PRIVATE test_checks.cpp)

target_link_libraries(test-check-input-data
	PRIVATE
		Boost::unit_test_framework
		Antares::checks
		Antares::study
		Antares::exception
)

add_test(NAME check-input-data COMMAND test-check-input-data)
set_tests_properties(check-input-data PROPERTIES LABELS unit)
