include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)


set(SRC_UTILS_FOR_COMPONENT_FILLER_TESTS
        component-filler-utils/variables-creators.h
        component-filler-utils/variables-creators.cpp
        component-filler-utils/objectives-creators.h
        component-filler-utils/objectives-creators.cpp
        component-filler-utils/constraints-creators.h
        component-filler-utils/constraints-creators.cpp)

add_boost_test(unit-tests-for-component-filler
        SRC
        test_main.cpp
        test_componentFiller_addVariables.cpp
        test_componentFiller_addConstraints.cpp
        test_componentFiller_addObjective.cpp
        test-component-filler-to-master-pb.cpp
        test_linearExpression.cpp
        test_readLinearExpressionVisitor.cpp
        test_readLinExprVisitor_sum_connections.cpp
        test_readLinearConstraintVisitor.cpp
        visitorFixture.hpp
        ${SRC_UTILS_FOR_COMPONENT_FILLER_TESTS}
        LIBS
        antares-study-system-model
        Antares::linear-problem-mpsolver-impl
        Antares::optim-model-filler
        Antares::linear-problem-data-impl
        Antares::tests::inmemory-modeler
        mock-modeler-objects
        test_utils_unit)
