
OMESSAGE("antares-study-finder")




# Le main
set(SRCS 	main.cpp)

if(WIN32)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/finder.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/finder.rc.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/win32/finder.rc")
	FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/finder.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
	SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/finder.rc")
endif(WIN32)


set(execname "antares-${ANTARES_PRG_VERSION}-study-finder")
add_executable(${execname}  ${SRCS})
install(TARGETS ${execname} EXPORT antares-study-finder DESTINATION bin)

INSTALL(EXPORT antares-study-finder
	FILE antares-study-finderConfig.cmake
	DESTINATION cmake
)

set(FINDER_LIBS
	libantares-core
	yuni-static-core
	${CMAKE_THREADS_LIBS_INIT}
)

target_link_libraries(${execname}
		PRIVATE
			${FINDER_LIBS}
		)

import_std_libs(${execname})
executable_strip(${execname})

