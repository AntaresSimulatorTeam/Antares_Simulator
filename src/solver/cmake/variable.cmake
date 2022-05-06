


set(SRC_VARIABLE
		variable/variable.h
		variable/variable.hxx
		variable/info.h
		variable/container.h
		variable/container.hxx
		variable/endoflist.h
		variable/state.h
		variable/state.cpp
		variable/state.hxx
		variable/area.h
		variable/area.hxx
		variable/area.inc.hxx
		variable/setofareas.h
		variable/setofareas.hxx
		variable/constants.h
		variable/categories.h
		variable/surveyresults.h
		variable/surveyresults/reportbuilder.hxx
		variable/surveyresults/surveyresults.h
		variable/surveyresults/data.h
		variable/surveyresults/surveyresults.cpp
		)
source_group("variable" FILES ${SRC_VARIABLE})

set(SRC_VARIABLE_STORAGE
		# Storage
		variable/storage/intermediate.h
		variable/storage/intermediate.hxx
		variable/storage/intermediate.cpp
		variable/storage/results.h
		variable/storage/empty.h
		variable/storage/raw.h
		variable/storage/raw.hxx
		variable/storage/rawdata.h
		variable/storage/rawdata.cpp
		variable/storage/minmax.h
		variable/storage/minmax.hxx
		variable/storage/minmax-data.h
		variable/storage/minmax-data.cpp
		variable/storage/average.h
		variable/storage/averagedata.h
		variable/storage/averagedata.cpp
		variable/storage/stdDeviation.h
		variable/storage/and.h
		variable/storage/fwd.h
		)
source_group("variable\\storage" FILES ${SRC_VARIABLE_STORAGE})

set(SRC_VARIABLE_COMMON
		# Commons
		variable/commons/join.h
		variable/commons/load.h
		variable/commons/wind.h
		variable/commons/hydro.h
		variable/commons/rowBalance.h
		variable/commons/psp.h
		variable/commons/miscGenMinusRowPSP.h
		variable/commons/solar.h
		variable/commons/spatial-aggregate.h
		variable/commons/links/links.cpp.inc.hxx
		variable/commons/links/links.h.inc.hxx
		variable/commons/links/links.hxx.inc.hxx
		)
source_group("variable\\common" FILES ${SRC_VARIABLE_COMMON})


set(SRC_VARIABLE_ADEQUACY_DRAFT
		variable/adequacy-draft/all.h
		variable/adequacy-draft/area.cpp
		variable/adequacy-draft/area.memory-estimation.cpp

		# Variables for Adequacy
		variable/adequacy-draft/lold_is.h
		variable/adequacy-draft/lold_cn.h
		variable/adequacy-draft/lold_is_system.h
		variable/adequacy-draft/lold_cn_system.h
		variable/adequacy-draft/ens_is.h
		variable/adequacy-draft/ens_cn.h
		variable/adequacy-draft/ens_is_system.h
		variable/adequacy-draft/ens_cn_system.h
		variable/adequacy-draft/lolp_is.h
		variable/adequacy-draft/lolp_cn.h
		variable/adequacy-draft/lolp_is_system.h
		variable/adequacy-draft/lolp_cn_system.h
		variable/adequacy-draft/minmarg_is.h
		variable/adequacy-draft/minmarg_cn.h
		variable/adequacy-draft/maxdepth_is.h
		variable/adequacy-draft/maxdepth_cn.h
		)
source_group("variable\\adequacy-draft" FILES ${SRC_VARIABLE_ADEQUACY_DRAFT})

set(SRC_VARIABLE_ADEQUACY
		variable/adequacy/all.h
		variable/adequacy/area.cpp
		variable/adequacy/area.memory-estimation.cpp
		variable/adequacy/links.h
		variable/adequacy/links.cpp
		variable/adequacy/spilledEnergy.h
		variable/adequacy/overallCost.h
		)
source_group("variable\\adequacy" FILES ${SRC_VARIABLE_ADEQUACY})


set(SRC_VARIABLE_ECONOMY
		variable/economy/all.h
		variable/economy/links.h
		variable/economy/links.cpp
		variable/economy/area.cpp
		variable/economy/area.memory-estimation.cpp

		# Variables for Economy
		variable/economy/co2.h
		variable/economy/max-mrg.h
		variable/economy/max-mrg.cpp
		variable/economy/price.h
		variable/economy/balance.h
		variable/economy/operatingCost.h
		variable/economy/overallCost.h
		variable/economy/nonProportionalCost.h
		variable/economy/hydrostorage.h
		variable/economy/pumping.h
		variable/economy/reservoirlevel.h
		variable/economy/inflow.h
		variable/economy/overflow.h
		variable/economy/waterValue.h
		variable/economy/hydroCost.h
		variable/economy/unsupliedEnergy.h
		variable/economy/domesticUnsuppliedEnergy.h
		variable/economy/lmrViolations.h
		variable/economy/spilledEnergy.h
		variable/economy/dispatchableGeneration.h
		variable/economy/productionByDispatchablePlant.h
        variable/economy/productionByRenewablePlant.h
		variable/economy/npCostByDispatchablePlant.h
		variable/economy/nbOfDispatchedUnitsByPlant.h
		variable/economy/nbOfDispatchedUnits.h
		variable/economy/lold.h
		variable/economy/lolp.h
		variable/economy/avail-dispatchable-generation.h
		variable/economy/dispatchable-generation-margin.h
		
		# Links
		variable/economy/links/flowLinear.h
		variable/economy/links/flowLinearAbs.h
		variable/economy/links/loopFlow.h
		variable/economy/links/flowQuad.h
		variable/economy/links/hurdleCosts.h
		variable/economy/links/congestionFee.h
		variable/economy/links/congestionFeeAbs.h
		variable/economy/links/marginalCost.h
		variable/economy/links/congestionProbability.h
		)
source_group("variable\\economy" FILES ${SRC_VARIABLE_ECONOMY})





#
# --- Library VARIABLES ---
#
add_library(libantares-solver-variable
		${SRC_VARIABLE}
		${SRC_VARIABLE_COMMON}
		${SRC_VARIABLE_STORAGE}
		${SRC_VARIABLE_ADEQUACY_DRAFT}
		${SRC_VARIABLE_ADEQUACY}
		${SRC_VARIABLE_ECONOMY}  )

target_include_directories(libantares-solver-variable PUBLIC .)
target_link_libraries(libantares-solver-variable PRIVATE libantares-core)

if(BUILD_SWAP)

    add_library(libantares-solver-variable-swap
            ${SRC_VARIABLE}
            ${SRC_VARIABLE_COMMON}
            ${SRC_VARIABLE_STORAGE}
            ${SRC_VARIABLE_ADEQUACY_DRAFT}
            ${SRC_VARIABLE_ADEQUACY}
            ${SRC_VARIABLE_ECONOMY}  )

    target_include_directories(libantares-solver-variable-swap PUBLIC .)
    target_link_libraries(libantares-solver-variable-swap PRIVATE libantares-core-swap)
    set_target_properties(libantares-solver-variable-swap PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")

endif()

add_library(libantares-solver-variable-info
		variable/adequacy-draft/all.h
		variable/adequacy/all.h
		variable/economy/all.h
		variable/economy/area.memory-estimation.cpp
		variable/adequacy-draft/area.memory-estimation.cpp
		variable/adequacy/area.memory-estimation.cpp
		variable/surveyresults.h
		variable/surveyresults/surveyresults.h
		variable/surveyresults/data.h
		variable/surveyresults/surveyresults.cpp
)
target_link_libraries(libantares-solver-variable-info PRIVATE libantares-core)

if(BUILD_SWAP)

    add_library(libantares-solver-variable-info-swap
            variable/adequacy-draft/all.h
            variable/adequacy/all.h
            variable/economy/all.h
            variable/economy/area.memory-estimation.cpp
            variable/adequacy-draft/area.memory-estimation.cpp
            variable/adequacy/area.memory-estimation.cpp
            variable/surveyresults.h
            variable/surveyresults/surveyresults.h
            variable/surveyresults/data.h
            variable/surveyresults/surveyresults.cpp
    )

    target_link_libraries(libantares-solver-variable-info-swap PRIVATE libantares-core-swap)
    set_target_properties(libantares-solver-variable-info-swap PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
    
endif()

