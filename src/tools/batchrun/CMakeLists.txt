project(AntaresBatchRun)
cmake_minimum_required(VERSION 2.8)

include(../../cmake/messages.cmake)
OMESSAGE("antares-batchrun")




Include(../../cmake/common-settings.cmake)




# Le main
set(SRCS  main.cpp)

if(WIN32)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/batchrun.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/batchrun.rc.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/win32/batchrun.rc")
	SET(SRCS ${SRCS} "win32/batchrun.rc")
endif(WIN32)



set(execname "antares-${ANTARES_PRG_VERSION}-batchrun")
add_executable(${execname} ${SRCS})

set(BATCHRUN_LIBS
		libantares-core
		libantares-core-calendar
		libantares-license
		yuni-static-core
		${CMAKE_THREADS_LIBS_INIT}
)

if (WIN32 OR WIN64)
	target_link_libraries(${execname} PRIVATE ${BATCHRUN_LIBS})
else()
	target_link_libraries(${execname} PRIVATE
		-Wl,--start-group
		${BATCHRUN_LIBS}
		-Wl,--end-group)
endif()


import_std_libs(${execname})
executable_strip(${execname})

