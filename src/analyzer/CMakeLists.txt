project(AntaresAnalyzer)

cmake_minimum_required(VERSION 2.8)

include(../cmake/messages.cmake)
OMESSAGE("antares-analyzer")

include(../cmake/common-settings.cmake)




# Le main
Set(SRCS 	main.cpp
			atsp/atsp.h
			atsp/atsp.hxx
			atsp/atsp.cpp
			atsp/load.cpp
			atsp/misc.cpp
			atsp/preflight.cpp
			atsp/correlations.cpp
			atsp/cache.cpp
			)


# The new ant library
include_directories("..")



if(WIN32)
	file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/analyzer.o")
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/win32/analyzer.rc.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/win32/analyzer.rc")

	if(MINGW)
		# resource compilation for mingw
		ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/win32/analyzer.o"
			COMMAND windres.exe "-I${CMAKE_CURRENT_SOURCE_DIR}"
			"-i${CMAKE_CURRENT_SOURCE_DIR}/win32/analyzer.rc"
			-o "${CMAKE_CURRENT_BINARY_DIR}/win32/analyzer.o")
		SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/analyzer.o")
	else(MINGW)
		SET(SRCS ${SRCS} "win32/analyzer.rc")
	endif(MINGW)
endif(WIN32)


add_executable(antares-${ANTARES_PRG_VERSION}-analyzer  ${SRCS})

target_link_libraries(antares-${ANTARES_PRG_VERSION}-analyzer
		libantares-license
		libantares-core
		libantares-core-calendar
		yuni-static-core
		${wxWidgets_LIBRARIES} ${CMAKE_THREADS_LIBS_INIT})
if(NOT WIN32)
	target_link_libraries(antares-${ANTARES_PRG_VERSION}-analyzer z dl ssh2 idn idn2 rt)
endif(NOT WIN32)


import_std_libs(antares-${ANTARES_PRG_VERSION}-analyzer)
executable_strip(antares-${ANTARES_PRG_VERSION}-analyzer)

