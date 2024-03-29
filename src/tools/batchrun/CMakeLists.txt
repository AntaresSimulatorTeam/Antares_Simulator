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
		"${CMAKE_CURRENT_BINARY_DIR}/win32/batchrun.rc")
	FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/batchrun.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
	SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/batchrun.rc")
endif(WIN32)



set(execname "antares-batchrun")
add_executable(${execname} ${SRCS})

install(TARGETS ${execname} EXPORT antares-batchrun DESTINATION bin)

INSTALL(EXPORT antares-batchrun
	FILE antares-batchrunConfig.cmake
	DESTINATION cmake
)

set(BATCHRUN_LIBS
		antares-core
		yuni-static-core
		${CMAKE_THREADS_LIBS_INIT}
		Antares::args_helper
		Antares::study
		Antares::resources
		Antares::locale
		Antares::locator
)

target_link_libraries(${execname}
		PRIVATE
			${BATCHRUN_LIBS}
		)

import_std_libs(${execname})
executable_strip(${execname})

