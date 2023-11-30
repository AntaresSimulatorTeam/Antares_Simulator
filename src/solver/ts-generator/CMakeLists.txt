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
	optimized-thermal-generator/Aux/AuxillaryStructures.h
	optimized-thermal-generator/Aux/AuxillaryStructures.cpp
	optimized-thermal-generator/Aux/AuxillaryFreeFunctions.h
	optimized-thermal-generator/Aux/AuxillaryFreeFunctions.cpp
	optimized-thermal-generator/Aux/pre-scenario-builder.h
	optimized-thermal-generator/Aux/pre-scenario-builder.cpp
	optimized-thermal-generator/Main/OptimizedGenerator.h
	optimized-thermal-generator/Main/OptimizedGenerator.cpp
	optimized-thermal-generator/Parameters/CalculateParameters.cpp
	optimized-thermal-generator/Parameters/CalculateParametersPerCluster.cpp
	optimized-thermal-generator/Parameters/Getters.cpp
	optimized-thermal-generator/Optimization/CreateVariables.cpp
	optimized-thermal-generator/Optimization/SetVariableBounds.cpp
	optimized-thermal-generator/Optimization/SetProblemConstraints.cpp
	optimized-thermal-generator/Optimization/SetProblemCost.cpp
	optimized-thermal-generator/Optimization/ResetProblem.cpp
	optimized-thermal-generator/Optimization/SolveProbem.cpp
	optimized-thermal-generator/Optimization/PrintProblemVarRes.cpp
	optimized-thermal-generator/PostOptimization/PostTimeStepOptimization.cpp
	optimized-thermal-generator/PostOptimization/PostScenarioOptimization.cpp

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
        utils #ortools-utils, not Antares::utils
)
