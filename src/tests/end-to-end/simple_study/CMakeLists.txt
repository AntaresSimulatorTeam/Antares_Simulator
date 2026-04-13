include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-simple-study
      SRC simple-study.cpp
      LIBS
      antares-solver-hydro
      antares-solver-variable
      antares-solver-simulation
      antares-solver-ts-generator
      model_antares
      Antares::tests::in-memory-study)
