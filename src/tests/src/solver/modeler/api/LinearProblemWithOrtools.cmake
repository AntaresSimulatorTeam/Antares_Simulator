add_executable(test-modeler-linear-problem testModelerLinearProblemWithOrtools.cpp)

target_include_directories(test-modeler-linear-problem
        PRIVATE
        "${src_solver_optimisation}"
)

target_link_libraries(test-modeler-linear-problem
        PRIVATE
        Boost::unit_test_framework
        Antares::modeler-ortools-impl
)

set_target_properties(test-modeler-linear-problem PROPERTIES FOLDER Unit-tests)
add_test(NAME modeler-linear-problem COMMAND test-modeler-linear-problem)
set_property(TEST modeler-linear-problem PROPERTY LABELS unit)