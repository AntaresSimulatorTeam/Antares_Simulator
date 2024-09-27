project(ts-generator)

#
# Time-Series Generators
#
set(SRC_GENERATORS
        include/antares/solver/ts-generator/generator.h
        include/antares/solver/ts-generator/generator.hxx
        generator.cpp
        availability.cpp
        hydro.cpp
)
source_group("ts-generator" FILES ${SRC_GENERATORS})


#
# XCast
#
set(SRC_XCAST
	include/antares/solver/ts-generator/xcast/constants.h
	include/antares/solver/ts-generator/xcast/studydata.h
	include/antares/solver/ts-generator/xcast/studydata.hxx
	include/antares/solver/ts-generator/law.h

	prepro.cpp

	include/antares/solver/ts-generator/prepro.h
	xcast/studydata.cpp
	xcast/gamma-euler.cpp
	xcast/gamma-inc.cpp
	xcast/core.cpp
    include/antares/solver/ts-generator/xcast/math.hxx
	xcast/math.cpp
	# XCAST
	include/antares/solver/ts-generator/xcast/xcast.h
	include/antares/solver/ts-generator/xcast/xcast.hxx
	xcast/xcast.cpp
)
source_group("ts-generator\\XCast" FILES ${SRC_XCAST})


#
# --- Library VARIABLES ---
#
add_library(antares-solver-ts-generator
        ${SRC_GENERATORS}
        ${SRC_XCAST})

target_link_libraries(antares-solver-ts-generator
        PRIVATE
        result_writer
        array
        benchmarking
        Antares::study
        Antares::misc
		antares-solver-simulation
)

target_include_directories(antares-solver-ts-generator
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
