add_executable(test-concurrency)

target_sources(test-concurrency PRIVATE test_concurrency.cpp)

target_link_libraries(test-concurrency
						PRIVATE
							Boost::unit_test_framework
							Antares::concurrency
)

add_test(NAME concurrency COMMAND test-concurrency)
