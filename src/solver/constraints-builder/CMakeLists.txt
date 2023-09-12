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
add_library(antares-solver-constraints-builder
		${SRC_CBUILDER} )


target_link_libraries(antares-solver-constraints-builder PRIVATE libantares-core
	PUBLIC
		Antares::study
)
