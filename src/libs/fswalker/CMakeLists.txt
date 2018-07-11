
OMESSAGE("Antares Filesystem Walker library")



add_library(libantares-fswalker STATIC
	fswalker.h
	fswalker.cpp
	statistics.h
	registry.inc.hxx
	filejob.inc.hxx
	job.h
	job.cpp
)

target_link_libraries(libantares-fswalker
	yuni-static-core)
