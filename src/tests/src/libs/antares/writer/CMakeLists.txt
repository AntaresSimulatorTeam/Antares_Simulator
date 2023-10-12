add_executable(test-writer)

target_sources(test-writer PRIVATE test_zip_writer.cpp)

target_link_libraries(test-writer
	PRIVATE
		Boost::unit_test_framework
		Antares::result_writer
		test_utils_unit
		MINIZIP::minizip
)

add_test(NAME writer COMMAND test-writer)
