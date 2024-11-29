project(variable)

set(SRC_VARIABLE
        include/antares/solver/variable/variable.h
        include/antares/solver/variable/variable.hxx
        include/antares/solver/variable/info.h
        include/antares/solver/variable/container.h
        include/antares/solver/variable/container.hxx
        include/antares/solver/variable/endoflist.h
        include/antares/solver/variable/state.h
        state.cpp
        include/antares/solver/variable/state.hxx
        include/antares/solver/variable/area.h
        include/antares/solver/variable/area.hxx
        include/antares/solver/variable/setofareas.h
        include/antares/solver/variable/setofareas.hxx
        include/antares/solver/variable/bindConstraints.h
        include/antares/solver/variable/bindConstraints.hxx
        include/antares/solver/variable/categories.h
        include/antares/solver/variable/surveyresults.h
        include/antares/solver/variable/surveyresults/reportbuilder.hxx
        include/antares/solver/variable/surveyresults/surveyresults.h
        include/antares/solver/variable/surveyresults/data.h
        surveyresults/surveyresults.cpp
)
source_group("variable" FILES ${SRC_VARIABLE})

set(SRC_VARIABLE_STORAGE
        # Storage
        include/antares/solver/variable/storage/intermediate.h
        include/antares/solver/variable/storage/intermediate.hxx
        storage/intermediate.cpp
        include/antares/solver/variable/storage/results.h
        include/antares/solver/variable/storage/empty.h
        include/antares/solver/variable/storage/raw.h
        include/antares/solver/variable/storage/raw.hxx
        include/antares/solver/variable/storage/rawdata.h
        storage/rawdata.cpp
        include/antares/solver/variable/storage/minmax.h
        include/antares/solver/variable/storage/minmax.hxx
        include/antares/solver/variable/storage/minmax-data.h
        storage/minmax-data.cpp
        include/antares/solver/variable/storage/average.h
        include/antares/solver/variable/storage/averagedata.h
        storage/averagedata.cpp
        include/antares/solver/variable/storage/stdDeviation.h
        include/antares/solver/variable/storage/fwd.h
)
source_group("variable\\storage" FILES ${SRC_VARIABLE_STORAGE})

set(SRC_VARIABLE_COMMON
        # Commons
        include/antares/solver/variable/commons/join.h
        include/antares/solver/variable/commons/load.h
        include/antares/solver/variable/commons/wind.h
        include/antares/solver/variable/commons/hydro.h
        include/antares/solver/variable/commons/rowBalance.h
        include/antares/solver/variable/commons/psp.h
        include/antares/solver/variable/commons/miscGenMinusRowPSP.h
        include/antares/solver/variable/commons/solar.h
        include/antares/solver/variable/commons/spatial-aggregate.h
        include/antares/solver/variable/commons/links/links.h
        include/antares/solver/variable/commons/links/links.hxx
)
source_group("variable\\common" FILES ${SRC_VARIABLE_COMMON})

set(SRC_VARIABLE_ADEQUACY
        include/antares/solver/variable/adequacy/all.h
        include/antares/solver/variable/adequacy/links.h
        include/antares/solver/variable/adequacy/spilledEnergy.h
        include/antares/solver/variable/adequacy/overallCost.h
)
source_group("variable\\adequacy" FILES ${SRC_VARIABLE_ADEQUACY})


set(SRC_VARIABLE_ECONOMY
        include/antares/solver/variable/economy/all.h
        include/antares/solver/variable/economy/links.h

        # Variables for Economy
		include/antares/solver/variable/economy/max-mrg-utils.h
		max-mrg-utils.cpp
        include/antares/solver/variable/economy/max-mrg.h
        include/antares/solver/variable/economy/price.h
        include/antares/solver/variable/economy/balance.h
        include/antares/solver/variable/economy/operatingCost.h
        include/antares/solver/variable/economy/overallCost.h
        include/antares/solver/variable/economy/nonProportionalCost.h
        include/antares/solver/variable/economy/hydrostorage.h
        include/antares/solver/variable/economy/pumping.h
        include/antares/solver/variable/economy/reservoirlevel.h
        include/antares/solver/variable/economy/inflow.h
        include/antares/solver/variable/economy/overflow.h
        include/antares/solver/variable/economy/waterValue.h
        include/antares/solver/variable/economy/hydroCost.h
        include/antares/solver/variable/economy/STSbyGroup.h
        include/antares/solver/variable/economy/STStorageInjectionByCluster.h
        include/antares/solver/variable/economy/STStorageWithdrawalByCluster.h
        include/antares/solver/variable/economy/STStorageLevelsByCluster.h
        include/antares/solver/variable/economy/STStorageCashFlowByCluster.h
        include/antares/solver/variable/economy/unsupliedEnergy.h
        include/antares/solver/variable/economy/unsupliedEnergyCsr.h
        include/antares/solver/variable/economy/domesticUnsuppliedEnergy.h
        include/antares/solver/variable/economy/localMatchingRuleViolations.h
        include/antares/solver/variable/economy/dtgMarginAfterCsr.h
        include/antares/solver/variable/economy/spilledEnergy.h
        include/antares/solver/variable/economy/dispatchableGeneration.h
        include/antares/solver/variable/economy/renewableGeneration.h
        include/antares/solver/variable/economy/thermalAirPollutantEmissions.h
        include/antares/solver/variable/economy/productionByDispatchablePlant.h
        include/antares/solver/variable/economy/productionByRenewablePlant.h
        include/antares/solver/variable/economy/npCostByDispatchablePlant.h
        include/antares/solver/variable/economy/nbOfDispatchedUnitsByPlant.h
        include/antares/solver/variable/economy/nbOfDispatchedUnits.h
        include/antares/solver/variable/economy/profitByPlant.h
        include/antares/solver/variable/economy/lold.h
        include/antares/solver/variable/economy/lolp.h
        include/antares/solver/variable/economy/avail-dispatchable-generation.h
        include/antares/solver/variable/economy/dispatchable-generation-margin.h

        # Links
        include/antares/solver/variable/economy/links/flowLinear.h
        include/antares/solver/variable/economy/links/flowLinearAbs.h
        include/antares/solver/variable/economy/links/loopFlow.h
        include/antares/solver/variable/economy/links/flowQuad.h
        include/antares/solver/variable/economy/links/hurdleCosts.h
        include/antares/solver/variable/economy/links/congestionFee.h
        include/antares/solver/variable/economy/links/congestionFeeAbs.h
        include/antares/solver/variable/economy/links/marginalCost.h
        include/antares/solver/variable/economy/links/congestionProbability.h
		include/antares/solver/variable/economy/overallCostCsr.h

        # Binding constraints
        include/antares/solver/variable/economy/bindingConstraints/bindingConstraintsMarginalCost.h
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
        ${SRC_VARIABLE_ECONOMY})

target_include_directories(antares-solver-variable
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs>
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(antares-solver-variable
        PUBLIC
        antares-solver-simulation
        PRIVATE
        antares-core
        Antares::study
)


add_library(antares-solver-variable-info
        include/antares/solver/variable/adequacy/all.h
        include/antares/solver/variable/economy/all.h
        include/antares/solver/variable/surveyresults.h
        include/antares/solver/variable/surveyresults/surveyresults.h
        include/antares/solver/variable/surveyresults/data.h
        surveyresults/surveyresults.cpp
)
target_include_directories(antares-solver-variable-info
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs>
)
target_link_libraries(antares-solver-variable-info
        PRIVATE
        antares-core
        Antares::study
        antares-solver-simulation
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
