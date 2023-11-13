project(ts-generator)

#
# Randomized-thermal-ts-generator
#
set(SRC_RANDOMIZEDGENERATORS
	randomized-thermal-generator/RandomizedGenerator.h
	randomized-thermal-generator/RandomizedGenerator.cpp
)
source_group("ts-generator\\randomized-thermal-generator" FILES ${SRC_RANDOMIZEDGENERATORS})

#
# Optimized-thermal-ts-generator
#
set(SRC_OPTIMIZEDGENERATORS
	optimized-thermal-generator/pre-scenario-builder.h
	optimized-thermal-generator/pre-scenario-builder.cpp
	optimized-thermal-generator/OptimizedGenerator.h
	optimized-thermal-generator/OptimizedGenerator.cpp
)
source_group("ts-generator\\optimized-thermal-generator" FILES ${SRC_OPTIMIZEDGENERATORS})

#
# Time-Series Generators
#
set(SRC_GENERATORS
	generator.h
	generator.hxx
	thermal.cpp
	hydro.cpp
)
source_group("ts-generator" FILES ${SRC_GENERATORS})


#
# XCast
#
set(SRC_XCAST
	xcast/constants.h
	xcast/studydata.h
	xcast/studydata.hxx
	xcast/studydata.cpp
	xcast/gamma-euler.cpp
	xcast/gamma-inc.cpp
	xcast/core.cpp
	xcast/math.hxx
	xcast/math.cpp
	# XCAST
	xcast/xcast.h
	xcast/xcast.hxx
	xcast/xcast.cpp
)
source_group("ts-generator\\XCast" FILES ${SRC_XCAST})



#
# --- Library VARIABLES ---
#
add_library(antares-solver-ts-generator
				${SRC_RANDOMIZEDGENERATORS}
				${SRC_OPTIMIZEDGENERATORS}
				${SRC_GENERATORS}
				${SRC_XCAST})

target_link_libraries(antares-solver-ts-generator
		PRIVATE
		result_writer
		array
		benchmarking
		Antares::study
)
