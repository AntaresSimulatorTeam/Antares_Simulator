# ====================================
# Tests on TimeSeries class
# ====================================
set(SRC_TIMESERIES_TESTS
	timeseries-tests.cpp
)
add_executable(timeseries-tests ${SRC_TIMESERIES_TESTS})

target_link_libraries(timeseries-tests
                      PRIVATE
                      Boost::unit_test_framework
                      Antares::series
)

# Storing timeseries-tests under the folder Unit-tests in the IDE
set_target_properties(timeseries-tests PROPERTIES FOLDER Unit-tests/timeseries-tests)

add_test(NAME timeseries-tests COMMAND timeseries-tests)

set_property(TEST timeseries-tests PROPERTY LABELS unit)
