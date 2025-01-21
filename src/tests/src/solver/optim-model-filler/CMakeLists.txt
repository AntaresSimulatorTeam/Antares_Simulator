include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)
add_boost_test(unit-tests-for-component-filler
        SRC
        test_main.cpp
        test_componentFiller.cpp
        test_linearExpression.cpp
        test_readLinearExpressionVisitor.cpp
        test_readLinearConstraintVisitor.cpp
        LIBS
        antares-study-system-model
        Antares::modeler-ortools-impl
        Antares::optim-model-filler
        data-series-lib
        test_utils_unit)

