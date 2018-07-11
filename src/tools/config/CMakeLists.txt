
OMESSAGE("antares-config")




# Le main
set(SRCS  main.cpp)


# The new ant library
include_directories("../../libs")

if(WIN32)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/config.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/config.rc.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/win32/config.rc")
	SET(SRCS ${SRCS} "win32/config.rc")
endif(WIN32)



set(execname "antares-${ANTARES_PRG_VERSION}-config")
add_executable(${execname} ${SRCS})

set(CONFIG_LIBS
		libantares-core
		yuni-static-core
		${CMAKE_THREADS_LIBS_INIT}
)

if (WIN32 OR WIN64)
	target_link_libraries(${execname} ${CONFIG_LIBS})
else()
	target_link_libraries(${execname} -Wl,--start-group ${CONFIG_LIBS} -Wl,--end-group)
endif()


import_std_libs(${execname})
executable_strip(${execname})

