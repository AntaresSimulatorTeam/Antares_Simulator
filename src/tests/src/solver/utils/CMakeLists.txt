include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-basis-status
        SRC
        test_main.cpp
        basis_status.cpp
        test_ortools_quadratic_wrapper.cpp
        INCLUDE
        "${CMAKE_SOURCE_DIR}/solver/utils"
        LIBS
        ortools::ortools
        Antares::solverUtils
        test_utils_unit)
