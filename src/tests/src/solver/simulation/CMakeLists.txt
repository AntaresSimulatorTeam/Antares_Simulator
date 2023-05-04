# Useful variables definitions
set(src_solver_simulation "${CMAKE_SOURCE_DIR}/solver/simulation")
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

set(SRC_TS_NUMBERS
	# For confort in IDE, but not necessary
	${src_solver_simulation}/timeseries-numbers.h
	
	# Necessary cpp files
	${src_solver_simulation}/timeseries-numbers.cpp
	${src_solver_simulation}/ITimeSeriesNumbersWriter.h)

add_executable(tests-ts-numbers tests-ts-numbers.cpp ${SRC_TS_NUMBERS})

target_include_directories(tests-ts-numbers
								PRIVATE
									"${src_solver_simulation}"
									"${src_libs_antares_study}"
)
target_link_libraries(tests-ts-numbers
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
							libantares-solver-simulation
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(tests-ts-numbers PROPERTIES FOLDER Unit-tests)

add_test(NAME ts-numbers COMMAND tests-ts-numbers)

set_property(TEST ts-numbers PROPERTY LABELS unit)

# ===================================
# Tests on area's store-timeseries-number
# ===================================
set(SRC_STORE_TS
		test-store-timeseries-number.cpp
		)
add_executable(test-store-timeseries-number ${SRC_STORE_TS})

target_link_libraries(test-store-timeseries-number
		PRIVATE
		Boost::unit_test_framework
		libantares-core
		libantares-solver-simulation
		)

# Linux
if(UNIX AND NOT APPLE)
	target_link_libraries(test-store-timeseries-number PRIVATE stdc++fs)
endif()

set_target_properties(test-store-timeseries-number PROPERTIES FOLDER Unit-tests)

add_test(NAME store-timeseries-number COMMAND test-store-timeseries-number)

set_property(TEST store-timeseries-number PROPERTY LABELS unit)

# ===================================
# Tests on time series
# ===================================
set(SRC_STORE_TS
		test-time_series.cpp
		)
add_executable(test-time_series ${SRC_STORE_TS})

target_link_libraries(test-time_series
		PRIVATE
		Boost::unit_test_framework
		libantares-core
		libantares-solver-simulation
		)

# Linux
if(UNIX AND NOT APPLE)
	target_link_libraries(test-time_series PRIVATE stdc++fs)
endif()


set_target_properties(test-time_series PROPERTIES FOLDER Unit-tests)

add_test(NAME time_series COMMAND test-time_series)

set_property(TEST time_series PROPERTY LABELS unit)

