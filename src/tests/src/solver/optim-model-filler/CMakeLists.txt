set(EXECUTABLE_NAME unit-tests-for-component-filler)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
        PRIVATE
        test_main.cpp
        test_componentFiller.cpp
        test_linearExpression.cpp
        test_readLinearExpressionVisitor.cpp
        test_readLinearConstraintVisitor.cpp
)
target_include_directories(${EXECUTABLE_NAME}
        PRIVATE
        "${src_solver_optimisation}"
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        antares-study-system-model
        Antares::modeler-ortools-impl
        Antares::optim-model-filler
        test_utils_unit
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)
add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
set_property(TEST ${EXECUTABLE_NAME} PROPERTY LABELS unit)

