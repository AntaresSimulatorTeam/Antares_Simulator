include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1)

FetchContent_MakeAvailable(Catch2)

#bigobj support needed for windows compilation
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

# Simple study

add_executable(tests-simple-study
  simple-study.cpp)

target_include_directories(tests-simple-study
							PRIVATE
							${CMAKE_SOURCE_DIR}/solver
							)

target_link_libraries(tests-simple-study
						PRIVATE
						libantares-core
						libantares-solver-main-economy
						libantares-solver-main-adequacy-draft
						libantares-solver-main-adequacy
						libantares-solver-hydro
						libantares-solver-aleatoire
						libantares-solver-variable
						libantares-solver-simulation
						libantares-solver-ts-generator
						libmodel_antares
						Boost::unit_test_framework
			)

add_test(NAME end-to-end-simple-study COMMAND tests-simple-study)

set_property(TEST end-to-end-simple-study PROPERTY LABELS end-to-end)

copy_dependency(sirius_solver tests-simple-study)

# Storing tests-simple-study under the folder Unit-tests in the IDE
set_target_properties(tests-simple-study PROPERTIES FOLDER Unit-tests)

# Playlist

add_executable(tests-playlist
			playlist.cpp)

target_include_directories(tests-playlist
							PRIVATE
                            ${CMAKE_SOURCE_DIR}/libs
                            ${CMAKE_SOURCE_DIR}/src/ext/yuni/src
                            Catch2::Catch2
							)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_link_libraries(tests-playlist
                      PRIVATE
                      stdc++fs
                      )
endif()

target_link_libraries(tests-playlist
						PRIVATE
						libantares-core
						libmodel_antares
                        Catch2::Catch2
			            )

target_compile_features(tests-playlist PRIVATE cxx_std_17)

add_test(NAME tests-playlist COMMAND tests-playlist)

set_property(TEST tests-playlist PROPERTY LABELS study-rw)
