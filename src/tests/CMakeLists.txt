cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

#copy test data to build directory
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data/" 		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/data/")

find_package(Boost COMPONENTS unit_test_framework filesystem regex REQUIRED)

# Make found targets globally available.
if (Boost_FOUND)
	set_target_properties(Boost::unit_test_framework PROPERTIES IMPORTED_GLOBAL TRUE)
	set_target_properties(Boost::filesystem PROPERTIES IMPORTED_GLOBAL TRUE)
	set_target_properties(Boost::regex PROPERTIES IMPORTED_GLOBAL TRUE)
endif()


add_subdirectory(src)

add_subdirectory(end-to-end)