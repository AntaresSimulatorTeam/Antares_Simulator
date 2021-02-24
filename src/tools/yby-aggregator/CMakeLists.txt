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

if(WIN32 OR WIN64)
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.o")
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.rc.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/win32/ybyaggregator.rc")
	FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
	SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/ybyaggregator.rc")
endif()


set(execname "antares-${ANTARES_PRG_VERSION}-ybyaggregator")
add_executable(${execname}  ${SRCS})
install(TARGETS ${execname} EXPORT antares-ybyaggregator DESTINATION bin)

INSTALL(EXPORT antares-ybyaggregator
	FILE antares-ybyaggregatorConfig.cmake
	DESTINATION cmake
)

set(YBY_AGGREGATOR_LIBS
	libantares-core-swap
	libantares-core-calendar
	yuni-static-core
	libantares-license
	${wxWidgets_LIBRARIES} ${CMAKE_THREADS_LIBS_INIT})

# The new ant library
target_include_directories(${execname}
		PRIVATE
			"${CMAKE_CURRENT_SOURCE_DIR}/libs"
		)

target_link_libraries(${execname}
		PRIVATE
			${YBY_AGGREGATOR_LIBS}
		)

set_target_properties(${execname} PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")

import_std_libs(${execname})
executable_strip(${execname})

