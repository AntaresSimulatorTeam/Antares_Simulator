set(EXECUTABLE_NAME test-modeler-linear-problem)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
        PRIVATE
        test_main.cpp
        testModelerLinearProblemWithOrtools.cpp
        testModelerLPbuilder.cpp
)
target_include_directories(${EXECUTABLE_NAME}
        PRIVATE
        "${src_solver_optimisation}"
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        Antares::modeler-ortools-impl
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)
## Make choice
## one command for all tests
add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
set_property(TEST ${EXECUTABLE_NAME} PROPERTY LABELS unit)

## one command for each test
add_test(NAME tests_on_OrtoolsLinearProblem COMMAND ${EXECUTABLE_NAME} --run_test=tests_on_OrtoolsLinearProblem)
set_property(TEST tests_on_OrtoolsLinearProblem PROPERTY LABELS unit)
add_test(NAME tests_on_linear_problem_builder COMMAND ${EXECUTABLE_NAME} --run_test=tests_on_linear_problem_builder)
set_property(TEST tests_on_linear_problem_builder PROPERTY LABELS unit)
