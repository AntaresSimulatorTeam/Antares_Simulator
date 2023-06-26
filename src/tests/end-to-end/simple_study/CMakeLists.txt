find_package(Boost COMPONENTS unit_test_framework REQUIRED)
enable_testing()

#bigobj support needed for windows compilation
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

add_executable(tests-simple-study
		simple-study.cpp
		)

target_link_libraries(tests-simple-study
						PRIVATE
						test_utils
						libantares-core
						libantares-solver-main-economy
						libantares-solver-main-adequacy
						libantares-solver-hydro
						libantares-solver-aleatoire
						libantares-solver-variable
						libantares-solver-simulation
						libantares-solver-ts-generator
						libmodel_antares
						${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			)
			
target_include_directories(tests-simple-study
							PRIVATE
							${CMAKE_SOURCE_DIR}/solver
							${CMAKE_CURRENT_SOURCE_DIR}/../utils
							)

target_compile_definitions(tests-simple-study PRIVATE "BOOST_TEST_DYN_LINK=1")

add_test(NAME end-to-end-simple-study COMMAND tests-simple-study)
set_property(TEST end-to-end-simple-study PROPERTY LABELS end-to-end)
set_target_properties(tests-simple-study PROPERTIES FOLDER Unit-tests/end_to_end)


copy_dependency(sirius_solver tests-simple-study)

# Storing tests-simple-study under the folder Unit-tests in the IDE

#----------------------------------------------------------