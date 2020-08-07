
OMESSAGE("antares-study-cleaner")


# Le main
Set(SRCS 	main.cpp
			)


# The new ant library
include_directories("../..")


if(WIN32 OR WIN64)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.rc.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/win32/cleaner.rc")
	FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
	SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/cleaner.rc")
endif()


set(execname "antares-${ANTARES_PRG_VERSION}-study-cleaner")
add_executable(${execname}  ${SRCS})
install(TARGETS ${execname} DESTINATION bin)

set(CLEANER_LIBS
	libantares-core
	libantares-license
	yuni-static-core
	${CMAKE_THREADS_LIBS_INIT}
)

target_link_libraries(${execname}
		PRIVATE
			${CLEANER_LIBS}
		)

import_std_libs(${execname})
executable_strip(${execname})

