set(EXECUTABLE_NAME tests-optim-api-ortools)
add_executable(${EXECUTABLE_NAME} testApiOrtoolsLinearProblem.cpp)

target_include_directories(${EXECUTABLE_NAME}
        PRIVATE
        "${src_solver_optimisation}"
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        Antares::optim-ortools-impl
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

add_test(NAME test-optim-api-ortools COMMAND ${EXECUTABLE_NAME})

set_property(TEST test-optim-api-ortools PROPERTY LABELS unit)
