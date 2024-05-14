add_executable(test-concurrency)

target_sources(test-concurrency PRIVATE test_concurrency.cpp)

target_link_libraries(test-concurrency
						PRIVATE
							antares-unit-test
							Antares::concurrency
)

add_test(NAME concurrency COMMAND test-concurrency)
set_property(TEST concurrency PROPERTY LABELS unit)
