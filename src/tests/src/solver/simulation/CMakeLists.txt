# -------------------------------
# Useful variables definitions
# -------------------------------
set(src_solver_simulation "${CMAKE_SOURCE_DIR}/solver/simulation")
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")


#########################################
# External dependencies needed by tests
#########################################

set(SRC_TS_NUMBERS_LIB
	# For confort in IDE, but not necessary
	${src_solver_simulation}/timeseries-numbers.h
	
	# Necessary cpp files
	${src_solver_simulation}/timeseries-numbers.cpp
)
	
add_library(lib-needed-by-ts-numbers ${SRC_TS_NUMBERS_LIB})

target_link_libraries(lib-needed-by-ts-numbers
						PUBLIC
							libantares-core
							libmodel_antares
)


######################
# Tests executable
######################

add_executable(tests-ts-numbers tests-ts-numbers.cpp)

target_include_directories(tests-ts-numbers
								PRIVATE
									"${src_solver_simulation}"
									"${src_libs_antares_study}"

)
target_link_libraries(tests-ts-numbers
						PRIVATE
							lib-needed-by-ts-numbers
							Boost::unit_test_framework
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(tests-ts-numbers PROPERTIES FOLDER Unit-tests)

add_test(NAME ts-numbers COMMAND tests-ts-numbers)

set_property(TEST ts-numbers PROPERTY LABELS TESTLABEL unit)

