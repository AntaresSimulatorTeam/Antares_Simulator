
OMESSAGE("antares-study-updater")


# Le main
set(SRCS 	main.cpp)



if(WIN32)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/updater.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/updater.rc.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/win32/updater.rc")
	SET(SRCS ${SRCS} "win32/updater.rc")
endif(WIN32)


set(execname "antares-${ANTARES_PRG_VERSION}-study-updater")
add_executable(${execname}  ${SRCS})

set(UPDATER_LIBS
		libantares-core
		libantares-license
		yuni-static-core
		${CMAKE_THREADS_LIBS_INIT}
		)

if (WIN32 OR WIN64)
	target_link_libraries(${execname} PRIVATE ${UPDATER_LIBS})
else()
	target_link_libraries(${execname} PRIVATE -Wl,--start-group ${UPDATER_LIBS} -Wl,--end-group)
endif()


import_std_libs(${execname})
executable_strip(${execname})

