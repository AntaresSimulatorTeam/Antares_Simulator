include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)
add_boost_test(unit-tests-for-component-filler
        SRC
        test_main.cpp
        test_componentFiller.cpp
        test_VariableDictionary.cpp
        test_linearExpression.cpp
        test_readLinearExpressionVisitor.cpp
        test_readLinExprVisitor_sum_connections.cpp
        test_readLinearConstraintVisitor.cpp
        LIBS
        Antares::optim-model-filler
        antares-study-system-model
        Antares::linear-problem-mpsolver-impl
        Antares::linear-problem-data-impl
        Antares::tests::inmemory-modeler
        test_utils_unit)

