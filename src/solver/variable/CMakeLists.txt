project(variable)

set(SRC_VARIABLE
		variable.h
		variable.hxx
		info.h
		container.h
		container.hxx
		endoflist.h
		state.h
		state.cpp
		state.hxx
		area.h
		area.hxx
		area.inc.hxx
		setofareas.h
		setofareas.hxx
		bindConstraints.h
		bindConstraints.hxx
		constants.h
		categories.h
		surveyresults.h
		surveyresults/reportbuilder.hxx
		surveyresults/surveyresults.h
		surveyresults/data.h
		surveyresults/surveyresults.cpp
		)
source_group("variable" FILES ${SRC_VARIABLE})

set(SRC_VARIABLE_STORAGE
		# Storage
		storage/intermediate.h
		storage/intermediate.hxx
		storage/intermediate.cpp
		storage/results.h
		storage/empty.h
		storage/raw.h
		storage/raw.hxx
		storage/rawdata.h
		storage/rawdata.cpp
		storage/minmax.h
		storage/minmax.hxx
		storage/minmax-data.h
		storage/minmax-data.cpp
		storage/average.h
		storage/averagedata.h
		storage/averagedata.cpp
		storage/stdDeviation.h
		storage/fwd.h
		)
source_group("variable\\storage" FILES ${SRC_VARIABLE_STORAGE})

set(SRC_VARIABLE_COMMON
		# Commons
		commons/join.h
		commons/load.h
		commons/wind.h
		commons/hydro.h
		commons/rowBalance.h
		commons/psp.h
		commons/miscGenMinusRowPSP.h
		commons/solar.h
		commons/spatial-aggregate.h
		commons/links/links.cpp.inc.hxx
		commons/links/links.h.inc.hxx
		commons/links/links.hxx.inc.hxx
		)
source_group("variable\\common" FILES ${SRC_VARIABLE_COMMON})

set(SRC_VARIABLE_ADEQUACY
		adequacy/all.h
		adequacy/area.cpp
		adequacy/area.memory-estimation.cpp
		adequacy/bc.memory-estimation.cpp
		adequacy/links.h
		adequacy/links.cpp
		adequacy/spilledEnergy.h
		adequacy/overallCost.h
		)
source_group("variable\\adequacy" FILES ${SRC_VARIABLE_ADEQUACY})


set(SRC_VARIABLE_ECONOMY
		economy/all.h
		economy/links.h
		economy/links.cpp
		economy/area.cpp
		economy/area.memory-estimation.cpp
		economy/bc.memory-estimation.cpp

		# Variables for Economy
		economy/max-mrg.h
		economy/max-mrg.cpp
		economy/price.h
		economy/balance.h
		economy/operatingCost.h
		economy/overallCost.h
		economy/nonProportionalCost.h
		economy/hydrostorage.h
		economy/pumping.h
		economy/reservoirlevel.h
		economy/inflow.h
		economy/overflow.h
		economy/waterValue.h
		economy/hydroCost.h
		economy/shortTermStorage.h
		economy/STStorageInjectionByCluster.h
		economy/STStorageWithdrawalByCluster.h
		economy/STStorageLevelsByCluster.h
		economy/unsupliedEnergy.h
		economy/domesticUnsuppliedEnergy.h
		economy/localMatchingRuleViolations.h
		economy/spilledEnergyAfterCSR.h
		economy/dtgMarginAfterCsr.h
		economy/spilledEnergy.h
		economy/dispatchableGeneration.h
		economy/renewableGeneration.h
		economy/thermalAirPollutantEmissions.h
		economy/productionByDispatchablePlant.h
        economy/productionByRenewablePlant.h
		economy/npCostByDispatchablePlant.h
		economy/nbOfDispatchedUnitsByPlant.h
		economy/nbOfDispatchedUnits.h
		economy/profitByPlant.h
		economy/lold.h
		economy/lolp.h
		economy/avail-dispatchable-generation.h
		economy/dispatchable-generation-margin.h

		# Links
		economy/links/flowLinear.h
		economy/links/flowLinearAbs.h
		economy/links/loopFlow.h
		economy/links/flowQuad.h
		economy/links/hurdleCosts.h
		economy/links/congestionFee.h
		economy/links/congestionFeeAbs.h
		economy/links/marginalCost.h
		economy/links/congestionProbability.h
		
		# Binding constraints
		economy/bindingConstraints/bindingConstraintsMarginalCost.h
		)
source_group("variable\\economy" FILES ${SRC_VARIABLE_ECONOMY})




#
# --- Library VARIABLES ---
#
add_library(antares-solver-variable
		${SRC_VARIABLE}
		${SRC_VARIABLE_COMMON}
		${SRC_VARIABLE_STORAGE}
		${SRC_VARIABLE_ADEQUACY}
		${SRC_VARIABLE_ECONOMY}  )

target_include_directories(antares-solver-variable
		PRIVATE
		${CMAKE_SOURCE_DIR}/libs
		PUBLIC
		.)
target_link_libraries(antares-solver-variable PRIVATE antares-core
	Antares::study
)


add_library(antares-solver-variable-info
		adequacy/all.h
		economy/all.h
		economy/area.memory-estimation.cpp
		adequacy/area.memory-estimation.cpp
		economy/bc.memory-estimation.cpp
		adequacy/bc.memory-estimation.cpp
		surveyresults.h
		surveyresults/surveyresults.h
		surveyresults/data.h
		surveyresults/surveyresults.cpp
)
target_include_directories(antares-solver-variable-info
		PRIVATE
		${CMAKE_SOURCE_DIR}/libs
)
target_link_libraries(antares-solver-variable-info PRIVATE antares-core
	Antares::study
)

