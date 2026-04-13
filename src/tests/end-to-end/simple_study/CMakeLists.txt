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

# antares-solver-simulation and model_antares are mutually dependent (circular).
# On GNU ld (single-pass), we need --start-group/--end-group to re-scan both.
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(tests-simple-study PRIVATE
        $<LINK_GROUP:RESCAN,antares-solver-simulation,model_antares>)
endif()
