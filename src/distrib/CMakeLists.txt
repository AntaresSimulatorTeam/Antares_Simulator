find_package(Git QUIET)

set(ANTARES_EXAMPLES_TAG "v7.2.0")

message(STATUS "Submodule examples update with ${ANTARES_EXAMPLES_TAG} tag")

# Define submodule tag

execute_process(COMMAND ${GIT_EXECUTABLE} submodule set-branch --branch ${ANTARES_EXAMPLES_TAG} .
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../resources/examples
				RESULT_VARIABLE GIT_SUBMOD_RESULT)
if(NOT GIT_SUBMOD_RESULT EQUAL "0")
	message(FATAL_ERROR "git submodule set-branch --branch ${ANTARES_EXAMPLES_TAG} failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
endif()

# Update example submodules as needed

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init 
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../resources/examples
				RESULT_VARIABLE GIT_SUBMOD_RESULT)
if(NOT GIT_SUBMOD_RESULT EQUAL "0")
	message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
endif()
		