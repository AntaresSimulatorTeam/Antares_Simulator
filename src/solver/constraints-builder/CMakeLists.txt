project(constraints-builder)
#
# Time-Series Generators
#
set(SRC_CBUILDER
	cbuilder.cpp
	cbuilder.h
	create.cpp
	grid.h
	grid.hxx
	load.cpp
)
source_group("constraints-builder" FILES ${SRC_CBUILDER})



#
# --- Library VARIABLES ---
#
add_library(libantares-solver-constraints-builder
		${SRC_CBUILDER} )


target_link_libraries(libantares-solver-constraints-builder PRIVATE libantares-core)
