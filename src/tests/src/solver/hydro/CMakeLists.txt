include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

set(src_solver_hydro "${CMAKE_SOURCE_DIR}/solver/hydro")

add_boost_test(unit-tests-hydro
        SRC
        monthly.cpp
        INCLUDE
        "${src_solver_hydro}"
        LIBS
        antares-solver-simulation)

