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

find_package(fmt REQUIRED)
target_link_libraries(antares-solver-ts-generator
        PUBLIC
        # studydata.h (PUBLIC header) includes correlation.h which uses these types;
        # consumers of this library need these include paths to compile studydata.h.
        Antares::correlation # studydata.h -> correlation.h
        Antares::array       # correlation.h -> antares/array/matrix.h
        Antares::inifile     # correlation.h -> antares/inifile/inifile.h
        Antares::enums       # antares/study/fwd.h -> antares/enums/Enum.hpp
        PRIVATE
        result_writer
        benchmarking
        Antares::study
        Antares::misc
		antares-solver-simulation
		fmt::fmt
)

target_include_directories(antares-solver-ts-generator
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # studydata.h and correlation.h include antares/study/*.h headers;
        # study cannot be a PUBLIC link dep (circular: study PUBLIC links ts-generator),
        # so we expose the include directory directly.
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/study/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
