
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

target_include_directories(libantares-fswalker
		PRIVATE
			${CMAKE_SOURCE_DIR}/libs
		)
target_link_libraries(libantares-fswalker
		PRIVATE
			yuni-static-core
			logs
		)
