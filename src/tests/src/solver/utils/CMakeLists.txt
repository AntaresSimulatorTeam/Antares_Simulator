include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(tests-solver-utils
        SRC
        test_main.cpp
        basis_status.cpp
        test_ortools_quadratic_wrapper.cpp
        test_ortools_utils.cpp
        test_quadratic_mps_writer.cpp
        INCLUDE
        "${CMAKE_SOURCE_DIR}/src/solver/utils"
        LIBS
        ortools::ortools
        Antares::solverUtils
        model_antares
        test_utils_unit)
