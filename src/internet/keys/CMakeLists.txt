

if (NOT WIN32 AND NOT WIN64)

	add_executable(rsakey2code  main.cpp)

	#TODO : needed for include yuni : why yuni must be include first ???
	target_include_directories(rsakey2code PRIVATE ${CMAKE_SOURCE_DIR}/ext/yuni/src)

	target_link_libraries(rsakey2code
			PRIVATE
				yuni-static-core
			)

endif()
