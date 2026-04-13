include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-simple-study
      SRC simple-study.cpp
      LIBS
      antares-solver-hydro
      antares-solver-variable
      $<LINK_GROUP:RESCAN,antares-solver-simulation,model_antares>
      antares-solver-ts-generator
      Antares::tests::in-memory-study)
