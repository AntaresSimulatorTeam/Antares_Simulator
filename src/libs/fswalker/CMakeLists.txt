
OMESSAGE("Antares Filesystem Walker library")



add_library(libantares-fswalker
	fswalker.h
	fswalker.cpp
	statistics.h
	registry.inc.hxx
	filejob.inc.hxx
	job.h
	job.cpp
)

#TODO : needed for include yuni : why yuni must be include first ???
#TODO : needed for include
target_include_directories(libantares-fswalker
		PRIVATE
			${CMAKE_SOURCE_DIR}/src/ext/yuni/src
			${CMAKE_SOURCE_DIR}/src/libs
		)
target_link_libraries(libantares-fswalker
		PRIVATE
			yuni-static-core
		)
