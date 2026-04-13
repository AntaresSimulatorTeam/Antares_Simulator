include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-binding_constraints
     SRC
     test_binding_constraints.cpp
     test_binding_constraints_with_mustrun.cpp
     LIBS
     $<LINK_GROUP:RESCAN,model_antares,antares-solver-simulation>
     antares-solver-hydro
     antares-solver-ts-generator
     Antares::tests::in-memory-study)

# antares-solver-simulation and model_antares are mutually dependent (circular).
# On GNU ld (single-pass), we need --start-group/--end-group to re-scan both.
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(tests-binding_constraints PRIVATE
        $<LINK_GROUP:RESCAN,antares-solver-simulation,model_antares>)
endif()
