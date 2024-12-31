include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-binding_constraints
     SRC test_binding_constraints.cpp
     LIBS
     model_antares
     antares-solver-simulation
     antares-solver-hydro
     antares-solver-ts-generator
     Antares::tests::in-memory-study)
