

if (NOT WIN32 AND NOT WIN64)

	add_executable(rsakey2code  main.cpp)
	target_link_libraries(rsakey2code yuni-static-core)

endif()
