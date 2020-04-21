
OMESSAGE("antares-study-cleaner")


# Le main
Set(SRCS 	main.cpp
			)


# The new ant library
include_directories("../..")


if(WIN32 OR WIN64)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.rc.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.rc")
	SET(SRCS ${SRCS} "win32/cleaner.rc")
endif()


set(execname "antares-${ANTARES_PRG_VERSION}-study-cleaner")
add_executable(${execname}  ${SRCS})

set(CLEANER_LIBS
	libantares-core
	libantares-license
	yuni-static-core
	${CMAKE_THREADS_LIBS_INIT}
)

if (WIN32 OR WIN64)
	target_link_libraries(${execname} PRIVATE ${CLEANER_LIBS})
else()
	target_link_libraries(${execname} PRIVATE -Wl,--start-group ${CLEANER_LIBS} -Wl,--end-group)
endif()

import_std_libs(${execname})
executable_strip(${execname})

