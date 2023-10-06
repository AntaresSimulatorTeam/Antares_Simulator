
add_executable(test-concurrency ${SRC_LINK_PROPERTIES})

target_sources(test-concurrency PRIVATE test_concurrency.cpp)

target_link_libraries(test-concurrency
						PRIVATE
							Boost::unit_test_framework
							Antares::concurrency
)

add_test(NAME concurrency COMMAND test-concurrency)
