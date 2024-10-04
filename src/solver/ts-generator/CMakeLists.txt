project(ts-generator)

#
# Randomized-thermal-ts-generator
#
set(SRC_RANDOMIZEDGENERATORS
	randomized-thermal-generator/RandomizedGenerator.h
	randomized-thermal-generator/RandomizedGenerator.cpp
)
source_group("ts-generator\\Randomized-thermal-generator" FILES ${SRC_RANDOMIZEDGENERATORS})

#
# Optimized-thermal-ts-generator
#
set(SRC_OPTIMIZEDGENERATORS
	optimized-thermal-generator/support/SupportStructures.h
	optimized-thermal-generator/support/SupportStructures.cpp
	optimized-thermal-generator/support/SupportFunctions.h
	optimized-thermal-generator/support/SupportFunctions.cpp
	optimized-thermal-generator/support/pre-scenario-builder.h
	optimized-thermal-generator/support/pre-scenario-builder.cpp
	optimized-thermal-generator/main/OptimizedGenerator.h
	optimized-thermal-generator/main/OptimizedGenerator.cpp
	optimized-thermal-generator/parameters/OptimizationParameters.h
	optimized-thermal-generator/parameters/CalculateParameters.cpp
	optimized-thermal-generator/parameters/CalculateParametersPerCluster.cpp
	optimized-thermal-generator/parameters/Getters.cpp
	optimized-thermal-generator/optimization/CreateVariables.cpp
	optimized-thermal-generator/optimization/SetVariableBounds.cpp
	optimized-thermal-generator/optimization/SetProblemConstraints.cpp
	optimized-thermal-generator/optimization/SetProblemCost.cpp
	optimized-thermal-generator/optimization/ResetProblem.cpp
	optimized-thermal-generator/optimization/SolveProbem.cpp
	optimized-thermal-generator/optimization/PrintProblemVarRes.cpp
	optimized-thermal-generator/postoptimization/PostTimeStepOptimization.cpp
	optimized-thermal-generator/postoptimization/PostScenarioOptimization.cpp
	optimized-thermal-generator/postoptimization/WriteResults.cpp

)
source_group("ts-generator\\Optimized-thermal-generator" FILES ${SRC_OPTIMIZEDGENERATORS})

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
        utils #ortools-utils, not Antares::utils
		Antares::misc
)
