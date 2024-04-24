add_subdirectory(concurrency)
add_subdirectory(writer)
add_subdirectory(study)


add_executable(test-utils test_utils.cpp)
target_link_libraries(test-utils
		PRIVATE
		Boost::unit_test_framework
		Antares::utils
		yuni-static-core
)
add_test(NAME test-utils COMMAND test-utils)
set_property(TEST test-utils PROPERTY LABELS unit)
