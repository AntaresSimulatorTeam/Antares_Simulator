

#
# Time-Series Generators
#
set(SRC_GENERATORS
	ts-generator/generator.h
	ts-generator/generator.hxx
	ts-generator/thermal.cpp
	ts-generator/hydro.cpp
)
source_group("ts-generator" FILES ${SRC_GENERATORS})


#
# XCast
#
set(SRC_XCAST
	ts-generator/xcast/constants.h
	ts-generator/xcast/studydata.h
	ts-generator/xcast/studydata.hxx
	ts-generator/xcast/studydata.cpp
	ts-generator/xcast/gamma-euler.cpp
	ts-generator/xcast/gamma-inc.cpp
	ts-generator/xcast/core.cpp
	ts-generator/xcast/math.hxx
	ts-generator/xcast/math.cpp
	# XCAST
	ts-generator/xcast/xcast.h
	ts-generator/xcast/xcast.hxx
	ts-generator/xcast/xcast.cpp
)
source_group("ts-generator\\XCast" FILES ${SRC_XCAST})




#
# --- Library VARIABLES ---
#
add_library(libantares-solver-ts-generator
		${SRC_GENERATORS}
		${SRC_XCAST} )

target_link_libraries(libantares-solver-ts-generator PRIVATE libantares-core)

if(BUILD_SWAP)
    add_library(libantares-solver-ts-generator-swap
            ${SRC_GENERATORS}
            ${SRC_XCAST} )
    target_link_libraries(libantares-solver-ts-generator-swap PRIVATE libantares-core-swap)

    set_target_properties(libantares-solver-ts-generator-swap PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")

endif()

