# Useful variables definitions
set(src_solver_optimisation "${CMAKE_SOURCE_DIR}/solver/optimisation")

set(SRC_ADQ_PATCH
	# For confort in IDE, but not necessary
	${src_solver_optimisation}/adequacy_patch.h
	
	# Necessary cpp files
	${src_solver_optimisation}/adequacy_patch.cpp
)

set(EXECUTABLE_NAME tests-adq-patch)

add_executable(${EXECUTABLE_NAME} adequacy_patch.cpp ${SRC_ADQ_PATCH})

target_include_directories(${EXECUTABLE_NAME}
								PRIVATE
									"${src_solver_optimisation}"
)

target_link_libraries(${EXECUTABLE_NAME}
                      PRIVATE
                      Boost::unit_test_framework
                      libantares-core
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

add_test(NAME test-adq-patch COMMAND ${EXECUTABLE_NAME})

set_property(TEST test-adq-patch PROPERTY LABELS unit)
