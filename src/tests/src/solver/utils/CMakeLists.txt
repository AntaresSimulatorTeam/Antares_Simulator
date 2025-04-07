include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-solver-utils
        SRC
        test_main.cpp
        basis_status.cpp
        test_ortools_quadratic_wrapper.cpp
        test_ortools_utils.cpp
        INCLUDE
        "${CMAKE_SOURCE_DIR}/solver/utils"
        LIBS
        # Until issue here is resolved, link the whole archive: https://github.com/google/or-tools/discussions/4538
        # TODO: keep an eye out for bug resolution in future versions of ortools (> 9.12)
        $<LINK_LIBRARY:WHOLE_ARCHIVE,ortools::ortools>
        Antares::solverUtils
        test_utils_unit)
