include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-options
        SRC
        tests-options.cpp
        INCLUDE
        "${CMAKE_SOURCE_DIR}/solver/misc"
        LIBS
        Antares::misc
        test_utils_unit)
