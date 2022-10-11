# Useful variables definitions
set(src_solver_simulation "${CMAKE_SOURCE_DIR}/solver/simulation")
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

set(SRC_TS_NUMBERS
	# For confort in IDE, but not necessary
	${src_solver_simulation}/timeseries-numbers.h
	
	# Necessary cpp files
	${src_solver_simulation}/timeseries-numbers.cpp
)

add_executable(tests-ts-numbers tests-ts-numbers.cpp ${SRC_TS_NUMBERS})

target_include_directories(tests-ts-numbers
								PRIVATE
									"${src_solver_simulation}"
									"${src_libs_antares_study}"
)
target_link_libraries(tests-ts-numbers
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(tests-ts-numbers PROPERTIES FOLDER Unit-tests)

add_test(NAME ts-numbers COMMAND tests-ts-numbers)

set_property(TEST ts-numbers PROPERTY LABELS unit)
