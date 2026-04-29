include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_subdirectory(remix-storage)

# Useful variables definitions
set(src_solver_simulation "${CMAKE_SOURCE_DIR}/solver/simulation")
set(src_solver_hydro "${CMAKE_SOURCE_DIR}/solver/hydro")
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")


add_boost_test(tests-ts-numbers
        SRC tests-ts-numbers.cpp
        INCLUDE
        "${src_solver_simulation}"
        "${src_libs_antares_study}"
        LIBS
        antares-solver-simulation
        test_utils_unit
        solver-lib)


# ===================================
# Tests on area's store-timeseries-number
# ===================================
add_boost_test(test-store-timeseries-number
        SRC test-store-timeseries-number.cpp
        LIBS
        test_utils_unit
        solver-lib)

# ===================================
# Tests on time series
# ===================================
add_boost_test(test-time_series
        SRC test-time_series.cpp
        LIBS
        test_utils_unit
        solver-lib)

# ===================================
# Tests on hydro final reservoir level functions
# ===================================
add_boost_test(test-hydro_final
        SRC
        test-hydro-final-reservoir-level-functions.cpp
        INCLUDE
        "${src_solver_simulation}"
        "${src_libs_antares_study}"
        "${src_solver_hydro}"
        LIBS
        test_utils_unit
        solver-lib)

# ===================================
# Tests on numspace manager
# ===================================
add_boost_test(test-numspace-manager
        SRC
        numspace_manager.cpp
        LIBS
        solver-lib)
