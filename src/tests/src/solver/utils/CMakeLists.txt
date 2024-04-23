# Useful variables definitions
set(src_solver_optimisation "${CMAKE_SOURCE_DIR}/solver/optimisation")

## BASIS STATUS
set(EXECUTABLE_NAME tests-basis-status)
add_executable(${EXECUTABLE_NAME} basis_status.cpp)


target_include_directories(${EXECUTABLE_NAME}
							PRIVATE
						    "${src_solver_optimisation}"
)

target_link_libraries(${EXECUTABLE_NAME}
                      PRIVATE
                      Boost::unit_test_framework
                      ortools::ortools
                      Antares::solverUtils
)

add_test(NAME test-basis-status COMMAND ${EXECUTABLE_NAME})
set_property(TEST test-basis-status PROPERTY LABELS unit)
