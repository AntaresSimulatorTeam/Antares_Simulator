
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

add_library(Antares::fswalker ALIAS antares-fswalker)

target_include_directories(antares-fswalker
		PUBLIC
			${CMAKE_SOURCE_DIR}/libs
		)
target_link_libraries(antares-fswalker
		PRIVATE
			yuni-static-core
			antares-core
			logs
		)
