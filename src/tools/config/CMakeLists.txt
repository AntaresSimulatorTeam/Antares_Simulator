
OMESSAGE("antares-config")




# Le main
set(SRCS  main.cpp)



if(WIN32)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/config.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/config.rc.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/win32/config.rc")
	FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/config.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
	SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/config.rc")
endif(WIN32)



set(execname "antares-${ANTARES_PRG_VERSION}-config")
add_executable(${execname} ${SRCS})
install(TARGETS ${execname} DESTINATION bin)

set(CONFIG_LIBS
		libantares-core
		yuni-static-core
		${CMAKE_THREADS_LIBS_INIT}
)

# The new ant library
target_include_directories(${execname}
		PRIVATE
			"${CMAKE_CURRENT_SOURCE_DIR}/libs"
		)

target_link_libraries(${execname}
		PRIVATE
			${CONFIG_LIBS}
		)

import_std_libs(${execname})
executable_strip(${execname})

