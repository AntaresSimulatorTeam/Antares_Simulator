find_package(Boost COMPONENTS unit_test_framework REQUIRED)
enable_testing()

#bigobj support needed for windows compilation
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

set(EXECUTABLE_NAME legacy-poc-simple-study)

add_executable(${EXECUTABLE_NAME}
		simple-study.cpp
		)

target_link_libraries(${EXECUTABLE_NAME}
						PRIVATE
						test_utils
						antares-solver-main-economy
						antares-solver-main-adequacy
						antares-solver-hydro
						antares-solver-variable
						antares-solver-simulation
						antares-solver-ts-generator
						model_antares
						${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			)

target_include_directories(${EXECUTABLE_NAME}
							PRIVATE
							${CMAKE_SOURCE_DIR}/solver
							${CMAKE_CURRENT_SOURCE_DIR}/../../end-to-end/utils
							)

add_test(NAME poc-legacy-study COMMAND ${EXECUTABLE_NAME})
set_property(TEST poc-legacy-study PROPERTY LABELS end-to-end)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests/poc-legacy-study)

# Storing tests-simple-study under the folder Unit-tests in the IDE

#----------------------------------------------------------
