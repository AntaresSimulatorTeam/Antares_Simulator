include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-duration-collector
               SRC test_duration_collector.cpp
               LIBS Antares::benchmarking)
