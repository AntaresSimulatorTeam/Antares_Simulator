
OMESSAGE("Antares Filesystem Walker library")



add_library(antares-fswalker
	fswalker.h
	fswalker.cpp
	statistics.h
	registry.inc.hxx
	filejob.inc.hxx
	job.h
	job.cpp
)

target_include_directories(antares-fswalker
		PRIVATE
			${CMAKE_SOURCE_DIR}/libs
		)
target_link_libraries(antares-fswalker
		PRIVATE
			yuni-static-core
			logs
		)
