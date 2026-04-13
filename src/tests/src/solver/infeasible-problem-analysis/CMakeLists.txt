include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)
add_boost_test(test-unfeasible-problem-analyzer
  SRC
  test-unfeasible-problem-analyzer.cpp
  LIBS
  infeasible_problem_analysis
        model_antares
        antares-solver-simulation
)

# antares-solver-simulation and model_antares are mutually dependent (circular).
# On GNU ld (single-pass), we need --start-group/--end-group to re-scan both.
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(test-unfeasible-problem-analyzer PRIVATE
        $<LINK_GROUP:RESCAN,antares-solver-simulation,model_antares>)
endif()
