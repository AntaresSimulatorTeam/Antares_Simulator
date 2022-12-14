# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

# ===========================================
# Tests on calculating forced parallel years
# ===========================================
set(SRC_PARALLEL_YEAR_CALC
    test-parallel-years-calculator.cpp
)
add_executable(test-parallel-years-calculator ${SRC_PARALLEL_YEAR_CALC})

target_include_directories(test-parallel-years-calculator
								PRIVATE
									"${src_libs_antares_study}"
									"${src_libs_antares_study}/parallel-years"
)

target_link_libraries(test-parallel-years-calculator
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
)
# Linux
if(UNIX AND NOT APPLE)
target_link_libraries(test-parallel-years-calculator PRIVATE stdc++fs)
endif()


# Storing test-parallel-years-calculator under the folder Unit-tests in the IDE
set_target_properties(test-parallel-years-calculator PROPERTIES FOLDER Unit-tests/parallel-years)

add_test(NAME parallel-years-calculator COMMAND test-parallel-years-calculator)

set_property(TEST parallel-years-calculator PROPERTY LABELS unit)