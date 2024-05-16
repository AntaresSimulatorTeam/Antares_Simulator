add_executable(test-concurrency)
add_compile_definitions(test-concurrency BOOST_TEST_DYN_LINK)
target_sources(test-concurrency PRIVATE test_concurrency.cpp)

target_link_libraries(test-concurrency
						PRIVATE
							Boost::unit_test_framework
							Antares::concurrency
)

add_test(NAME concurrency COMMAND test-concurrency)
set_property(TEST concurrency PROPERTY LABELS unit)
