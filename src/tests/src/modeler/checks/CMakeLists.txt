include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-modeler-checks
        SRC
        testCheckLocation.cpp
        LIBS
        Antares::modeler-checks
        Antares::modeler-lib
        test_utils_unit
)
