include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-scenario-builder-expression
        SRC test_scenario_builder_expr.cpp
        LIBS
        Antares::scenario-builder-antlr
        Antares::scenario-group-parser
)