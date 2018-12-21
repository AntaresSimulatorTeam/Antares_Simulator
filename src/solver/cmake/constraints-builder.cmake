

#
# Time-Series Generators
#
set(SRC_CBUILDER
	constraints-builder/cbuilder.cpp
	constraints-builder/cbuilder.h
	constraints-builder/create.cpp
	constraints-builder/grid.h
	constraints-builder/grid.hxx
	constraints-builder/load.cpp
)
source_group("constraints-builder" FILES ${SRC_CBUILDER})



#
# --- Library VARIABLES ---
#
add_library(libantares-solver-constraints-builder
		${SRC_CBUILDER} )
		
set_target_properties(libantares-solver-constraints-builder PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")


