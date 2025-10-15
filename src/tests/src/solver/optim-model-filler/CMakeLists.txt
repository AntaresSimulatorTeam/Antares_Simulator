include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)
add_boost_test(unit-tests-for-component-filler
        SRC
        test_main.cpp
        test_componentFiller.cpp
        test_linearExpression.cpp
        test_readLinearExpressionVisitor.cpp
        test_readLinExprVisitor_sum_connections.cpp
        test_readLinearConstraintVisitor.cpp
	visitorFixture.hpp
        LIBS
        antares-study-system-model
        Antares::linear-problem-mpsolver-impl
        Antares::optim-model-filler
        Antares::linear-problem-data-impl
        Antares::tests::inmemory-modeler
        test_utils_unit)

