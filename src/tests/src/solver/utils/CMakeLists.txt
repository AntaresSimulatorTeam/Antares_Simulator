set(src_utils "${CMAKE_SOURCE_DIR}/solver/utils")

set(EXECUTABLE_NAME tests-basis-status)
add_executable(${EXECUTABLE_NAME} basis_status.cpp)
add_compile_definitions(${EXECUTABLE_NAME} BOOST_TEST_DYN_LINK)

target_include_directories(${EXECUTABLE_NAME}
							PRIVATE
						    "${src_utils}" # basis_status_impl.h is private
)

target_link_libraries(${EXECUTABLE_NAME}
                      PRIVATE
                      Boost::unit_test_framework
                      ortools::ortools
                      Antares::solverUtils
)

add_test(NAME test-basis-status COMMAND ${EXECUTABLE_NAME})
set_property(TEST test-basis-status PROPERTY LABELS unit)
