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
add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
set_property(TEST ${EXECUTABLE_NAME} PROPERTY LABELS unit)

