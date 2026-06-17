include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(test-concurrency
               SRC test_concurrency.cpp
               LIBS Antares::concurrency)
