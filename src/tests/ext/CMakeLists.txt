
if(BUILD_BOOST)
	message("")
	message("**********************************************************************")
	message("** Download, build and installation of Boost ...")
	message("**********************************************************************")

	# Creating the cmake file that is about to download, build and install boost
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in
		${CMAKE_CURRENT_BINARY_DIR}/CMakeLists.txt @ONLY
		)

	# Configuration step of the previously created cmake file
	execute_process(
		COMMAND ${CMAKE_COMMAND} -H. -B project_build -G "${CMAKE_GENERATOR}"
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)
	if(result)
		message(FATAL_ERROR "CMake configuration step for Boost Test failed: ${result}")
	endif()

	# Build step of the previously created cmake file (download, build and install are processed here)
	execute_process(
		COMMAND ${CMAKE_COMMAND} --build project_build --config ${CMAKE_BUILD_TYPE}
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)
	if(result)
		message(FATAL_ERROR "CMake build step for Boost Test failed: ${result}")
	endif()

	message("**********************************************************************")
	message("** Finished download, build and installation of Boost.")
	message("**********************************************************************")
	message("")
endif()