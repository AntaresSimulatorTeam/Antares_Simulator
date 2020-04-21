project(AntaresStudyYearByYearAggregator)
cmake_minimum_required(VERSION 2.8)

include(../../cmake/messages.cmake)
OMESSAGE("antares-ybyaggregator")




include(../../cmake/common-settings.cmake)




# Le main
set(SRCS
		main.cpp
		datafile.h
		result.h
		result.cpp
		output.h
		output.cpp
		job.h
		job.hxx
		job.cpp
		progress.h
		progress.hxx
		progress.cpp
			)


# The new ant library
include_directories("../..")



if(WIN32 OR WIN64)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.rc.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.rc")
	SET(SRCS ${SRCS} "win32/ybyaggregator.rc")
endif()


set(execname "antares-${ANTARES_PRG_VERSION}-ybyaggregator")
add_executable(${execname}  ${SRCS})


set(YBY_AGGREGATOR_LIBS
	libantares-core-swap
	libantares-core-calendar
	yuni-static-core
	libantares-license
	${wxWidgets_LIBRARIES} ${CMAKE_THREADS_LIBS_INIT})

if (WIN32 OR WIN64)
	target_link_libraries(${execname} PRIVATE ${YBY_AGGREGATOR_LIBS})
else()
	target_link_libraries(${execname} PRIVATE -Wl,--start-group ${YBY_AGGREGATOR_LIBS} -Wl,--end-group)
endif()

set_target_properties(${execname} PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")

import_std_libs(${execname})
executable_strip(${execname})

