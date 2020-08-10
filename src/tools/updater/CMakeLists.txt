
OMESSAGE("antares-study-updater")


# Le main
set(SRCS 	main.cpp)


if(WIN32)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/updater.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/updater.rc.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/win32/updater.rc")
	FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/updater.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
	SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/updater.rc")
endif(WIN32)


set(execname "antares-${ANTARES_PRG_VERSION}-study-updater")
add_executable(${execname}  ${SRCS})
install(TARGETS ${execname} DESTINATION bin)

set(UPDATER_LIBS
		libantares-core
		libantares-license
		yuni-static-core
		${CMAKE_THREADS_LIBS_INIT}
		)

target_link_libraries(${execname}
		PRIVATE
			${UPDATER_LIBS}
		)

import_std_libs(${execname})
executable_strip(${execname})

