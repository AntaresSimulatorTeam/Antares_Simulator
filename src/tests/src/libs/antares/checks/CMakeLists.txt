include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(tests-checks
        SRC
        tests-checks.cpp
        INCLUDE
        "${CMAKE_SOURCE_DIR}/src/libs/antares/study/include"
        "${CMAKE_SOURCE_DIR}/src/libs/antares/include"
        LIBS
        Antares::checks
        yuni-static-core
        test_utils_unit
        Antares::enums
)
