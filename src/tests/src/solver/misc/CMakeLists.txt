include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(tests-options
        SRC
        tests-options.cpp
        LIBS
        Antares::misc
        test_utils_unit)
