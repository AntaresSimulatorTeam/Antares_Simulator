# Hydro data reader
set(SRC_HYDRO_READER
	test-hydroreader-class.cpp)

add_executable(test-hydro-reader ${SRC_HYDRO_READER})

target_link_libraries(test-hydro-reader
	PRIVATE
	Boost::unit_test_framework
	Antares::study
	test_utils_unit
)

# Linux
if(UNIX AND NOT APPLE)
	target_link_libraries(test-hydro-reader PRIVATE stdc++fs)
endif()

set_target_properties(test-hydro-reader PROPERTIES FOLDER Unit-tests/hydro)
add_test(NAME hydro-reader COMMAND test-hydro-reader)
set_property(TEST hydro-reader PROPERTY LABELS unit)


# Hydro series
set(SRC_HYDRO_SERIES
	test-hydro-series.cpp)

add_executable(test-hydro-series ${SRC_HYDRO_SERIES})

target_link_libraries(test-hydro-series
	PRIVATE
	Boost::unit_test_framework
	Antares::study
	test_utils_unit
)

# Linux
if(UNIX AND NOT APPLE)
	target_link_libraries(test-hydro-series PRIVATE stdc++fs)
endif()

set_target_properties(test-hydro-series PROPERTIES FOLDER Unit-tests/hydro)
add_test(NAME hydro-series COMMAND test-hydro-series)
set_property(TEST hydro-series PROPERTY LABELS unit)