set(SRC_STUDY_SCENARIO_BUILDER
        scenario-builder/scBuilderDataInterface.h
        scenario-builder/TSnumberData.h
        scenario-builder/TSnumberData.cpp
        scenario-builder/hydroLevelsData.h
        scenario-builder/hydroLevelsData.cpp
        scenario-builder/rules.h
        scenario-builder/rules.hxx
        scenario-builder/rules.cpp
        scenario-builder/sets.h
        scenario-builder/sets.hxx
        scenario-builder/sets.cpp
        scenario-builder/updater.hxx
        scenario-builder/scBuilderUtils.h
        scenario-builder/scBuilderUtils.cpp
        scenario-builder/BindingConstraintsTSNumbersData.cpp
        scenario-builder/NTCTSNumberData.cpp
        scenario-builder/NTCTSNumberData.h
        scenario-builder/BindingConstraintsTSNumbersData.h
        scenario-builder/RenewableTSNumberData.h
        scenario-builder/RenewableTSNumberData.cpp
        scenario-builder/ThermalTSNumberData.cpp
        scenario-builder/ThermalTSNumberData.h
        scenario-builder/HydroTSNumberData.h
        scenario-builder/HydroTSNumberData.cpp
        scenario-builder/SolarTSNumberData.cpp
        scenario-builder/solarTSNumberData.h
        scenario-builder/WindTSNumberData.h
        scenario-builder/WindTSNumberData.cpp
        scenario-builder/LoadTSNumberData.h
        scenario-builder/LoadTSNumberData.cpp
        scenario-builder/applyToMatrix.hxx
)
source_group("study\\scenario builder" FILES ${SRC_MATRIX})

set(SRC_STUDY_ACTIONS
        action/fwd.h
        action/action.h
        action/action.hxx
        action/action.cpp
        action/text.h
        action/text.hxx
        action/settings/suffix-for-area-names.h
        action/settings/suffix-for-area-names.hxx
        action/settings/suffix-for-area-names.cpp
        action/settings/suffix-for-cluster-names.h
        action/settings/suffix-for-cluster-names.hxx
        action/settings/suffix-for-cluster-names.cpp
        action/settings/suffix-for-constraint-names.h
        action/settings/suffix-for-constraint-names.hxx
        action/settings/suffix-for-constraint-names.cpp
        action/settings/decal-area-position.h
        action/settings/decal-area-position.hxx
        action/settings/decal-area-position.cpp
        action/behavior.h
        action/behavior.cpp
        action/state.h
        action/state.cpp
        action/context.h
        action/context.cpp)
source_group("study\\actions" FILES ${SRC_STUDY_ACTIONS})

set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_AREA
        action/handler/antares-study/area/create.h
        action/handler/antares-study/area/create.hxx
        action/handler/antares-study/area/create.cpp
        action/handler/antares-study/area/position.h
        action/handler/antares-study/area/position.cpp
        action/handler/antares-study/area/filtering.h
        action/handler/antares-study/area/filtering.cpp
        action/handler/antares-study/area/color.h
        action/handler/antares-study/area/color.cpp
        action/handler/antares-study/area/nodal-optimization.h
        action/handler/antares-study/area/nodal-optimization.cpp
        action/handler/antares-study/area/reserves.h
        action/handler/antares-study/area/reserves.cpp
        action/handler/antares-study/area/misc-gen.h
        action/handler/antares-study/area/misc-gen.cpp
        action/handler/antares-study/area/timeseries.h
        action/handler/antares-study/area/timeseries.cpp
        action/handler/antares-study/area/ts-generator.h
        action/handler/antares-study/area/ts-generator.cpp
        action/handler/antares-study/area/ts-node.h
        action/handler/antares-study/area/ts-node.hxx
        action/handler/antares-study/area/ts-node.cpp
        action/handler/antares-study/area/correlation.h
        action/handler/antares-study/area/correlation.cpp
        action/handler/antares-study/area/correlation-post.h
        action/handler/antares-study/area/correlation-post.hxx
        action/handler/antares-study/area/correlation-post.cpp

        action/handler/antares-study/area/allocation-hydro.h
        action/handler/antares-study/area/allocation-hydro.cpp
        action/handler/antares-study/area/allocation-hydro-post.h
        action/handler/antares-study/area/allocation-hydro-post.hxx
        action/handler/antares-study/area/allocation-hydro-post.cpp)
source_group("study\\actions\\handler\\antares study\\area" FILES ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_AREA})

set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_LINK
        action/handler/antares-study/link/create.h
        action/handler/antares-study/link/create.hxx
        action/handler/antares-study/link/create.cpp
        action/handler/antares-study/link/ntc.h
        action/handler/antares-study/link/ntc.cpp
        action/handler/antares-study/link/hurdles-cost.h
        action/handler/antares-study/link/hurdles-cost.cpp
        action/handler/antares-study/link/impedances.h
        action/handler/antares-study/link/impedances.cpp
        action/handler/antares-study/link/comments.h
        action/handler/antares-study/link/comments.hxx
        action/handler/antares-study/link/comments.cpp
        action/handler/antares-study/link/copper-plate.h
        action/handler/antares-study/link/copper-plate.cpp
        action/handler/antares-study/link/filtering.h
        action/handler/antares-study/link/filtering.cpp
        action/handler/antares-study/link/unload-data.h
        action/handler/antares-study/link/unload-data.hxx
        action/handler/antares-study/link/unload-data.cpp)
source_group("study\\actions\\handler\\antares study\\link" FILES ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_LINK})

set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_THERMAL
        action/handler/antares-study/thermal-cluster/create.h
        action/handler/antares-study/thermal-cluster/create.hxx
        action/handler/antares-study/thermal-cluster/create.cpp
        action/handler/antares-study/thermal-cluster/root-node.h
        action/handler/antares-study/thermal-cluster/root-node.hxx
        action/handler/antares-study/thermal-cluster/root-node.cpp
        action/handler/antares-study/thermal-cluster/common.h
        action/handler/antares-study/thermal-cluster/common.cpp
)
source_group("study\\action\\handler\\sntares study\\thermal cluster" FILES ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_THERMAL})


set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_CONSTRAINT
        action/handler/antares-study/constraint/create.h
        action/handler/antares-study/constraint/create.hxx
        action/handler/antares-study/constraint/create.cpp
        action/handler/antares-study/constraint/comments.h
        action/handler/antares-study/constraint/comments.hxx
        action/handler/antares-study/constraint/comments.cpp
        action/handler/antares-study/constraint/data.h
        action/handler/antares-study/constraint/data.hxx
        action/handler/antares-study/constraint/data.cpp
        action/handler/antares-study/constraint/weights.h
        action/handler/antares-study/constraint/weights.cpp
        action/handler/antares-study/constraint/offsets.h
        action/handler/antares-study/constraint/offsets.cpp
        action/handler/antares-study/constraint/enabled.h
        action/handler/antares-study/constraint/enabled.hxx
        action/handler/antares-study/constraint/enabled.cpp
)
source_group("study\\action\\handler\\antares study\\constraint" FILES ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_CONSTRAINT})


set(SRC_AREAS
        area.h
        area/constants.h
        area/area.h
        area/area.hxx
        area/scratchpad.h
        area/scratchpad.cpp
        area/area.cpp
        area/links.cpp
        area/links.h
        area/store-timeseries-numbers.cpp
        area/store-timeseries-numbers.h
        area/store-timeseries-numbers.hxx
        area/list.cpp
)
source_group("study\\area" FILES ${SRC_AREAS})


set(SRC_STUDY_AREA_UI
        area/ui.h
        area/ui.cpp
)
source_group("study\\area\\ui" FILES ${SRC_STUDY_AREA_UI})


set(SRC_STUDY_PART_COMMON
        parts/common/series.h
        parts/common/series.hxx
        parts/common/series.cpp
        parts/common/cluster.cpp
        parts/common/cluster.h
        parts/common/cluster_list.h
        parts/common/cluster_list.cpp
)
source_group("study\\part\\common" FILES ${SRC_STUDY_PART_COMMON})

set(SRC_STUDY_PART_THERMAL
        parts/thermal/container.h
        parts/thermal/container.hxx
        parts/thermal/container.cpp
        parts/thermal/prepro.h
        parts/thermal/prepro.hxx
        parts/thermal/prepro.cpp
        parts/thermal/ecoInput.h
        parts/thermal/ecoInput.cpp
        parts/thermal/cluster.h
        parts/thermal/cluster.hxx
        parts/thermal/cluster.cpp
        parts/thermal/cluster_list.h
        parts/thermal/cluster_list.cpp
        parts/thermal/pollutant.h
        parts/thermal/pollutant.cpp
)
source_group("study\\part\\thermal" FILES ${SRC_STUDY_PART_THERMAL})

set(SRC_STUDY_PART_RENEWABLE
        parts/renewable/container.h
        parts/renewable/container.hxx
        parts/renewable/container.cpp
        parts/renewable/cluster.h
        parts/renewable/cluster.cpp
        parts/renewable/cluster_list.h
        parts/renewable/cluster_list.cpp
)
source_group("study\\part\\renewable" FILES ${SRC_STUDY_PART_RENEWABLE})

set(SRC_STUDY_PART_SHORT_TERM_STORAGE
        parts/short-term-storage/container.h
        parts/short-term-storage/container.cpp
        parts/short-term-storage/properties.h
        parts/short-term-storage/properties.cpp
        parts/short-term-storage/series.cpp
        parts/short-term-storage/series.h
        parts/short-term-storage/cluster.h
        parts/short-term-storage/cluster.cpp
        parts/short-term-storage/STStorageOutputCaptions.h
        parts/short-term-storage/STStorageOutputCaptions.cpp
)
source_group("study\\part\\short-term-storage" FILES ${SRC_STUDY_PART_SHORT_TERM_SOTRAGE})

set(SRC_STUDY_PART_WIND
        parts/wind/container.h
        parts/wind/container.cpp
        parts/wind/series.h
        parts/wind/series.hxx
        parts/wind/series.cpp
        parts/wind/prepro.h
        parts/wind/prepro.hxx
        parts/wind/prepro.cpp
)
source_group("study\\part\\wind" FILES ${SRC_STUDY_PART_WIND})

set(SRC_STUDY_PART_HYDRO
        parts/hydro/container.h
        parts/hydro/container.cpp
        parts/hydro/series.h
        parts/hydro/series.hxx
        parts/hydro/series.cpp
        parts/hydro/prepro.h
        parts/hydro/prepro.cpp
        parts/hydro/allocation.h
        parts/hydro/allocation.hxx
        parts/hydro/allocation.cpp
)
source_group("study\\part\\hydro" FILES ${SRC_STUDY_PART_HYDRO})

set(SRC_STUDY_UI
        ui-runtimeinfos.h
        ui-runtimeinfos.cpp
)
source_group(study\\UI FILES ${SRC_STUDY_UI})

set(SRC_STUDY_PARAMETERS
        parameters.h
        parameters.cpp
        parameters/adq-patch-params.h
        parameters/adq-patch-params.cpp
        UnfeasibleProblemBehavior.cpp
        # TODO JMK : should be in a target_include_directories : but need to remove all .h from all SRC
        UnfeasibleProblemBehavior.hpp
)
source_group("study\\general data" FILES ${SRC_STUDY_PARAMETERS})


set(SRC_STUDY_FINDER
        finder.h
        finder/finder.cpp
        finder/finder.h
)
source_group("study\\finder" FILES ${SRC_STUDY_FINDER})


set(SRC_STUDY_BINDING_CONSTRAINT
        constraint.h
        binding_constraint/BindingConstraint.h
        binding_constraint/BindingConstraint.hxx
        binding_constraint/BindingConstraint.cpp
        binding_constraint/BindingConstraintsRepository.h
        binding_constraint/BindingConstraintsRepository.h
        binding_constraint/BindingConstraintsRepository.hxx
        binding_constraint/BindingConstraintsRepository.cpp
        binding_constraint/BindingConstraintLoader.cpp
        binding_constraint/BindingConstraintLoader.h
        binding_constraint/BindingConstraintSaver.cpp
        binding_constraint/BindingConstraintSaver.h
        binding_constraint/EnvForLoading.h
        binding_constraint/BindingConstraintStructures.h
        binding_constraint/BindingConstraintGroup.cpp
        binding_constraint/BindingConstraintGroup.h
        binding_constraint/BindingConstraintGroupRepository.cpp
        binding_constraint/BindingConstraintGroupRepository.h
)

source_group("study\\constraint" FILES ${SRC_STUDY_BINDING_CONSTRAINT})


set(SRC_XCAST
        xcast.h
        xcast/xcast.h
        xcast/xcast.hxx
        xcast/xcast.cpp
)
source_group("study\\xcast" FILES ${SRC_XCAST})

set(SRC_STUDY
        # all
        parts/parts.h

        # Load
        parts/load/container.h
        parts/load/container.cpp
        parts/load/series.h
        parts/load/series.hxx
        parts/load/series.cpp
        parts/load/prepro.cpp
        parts/load/prepro.h
        parts/load/prepro.hxx

        # Solar
        parts/solar/container.h
        parts/solar/container.cpp
        parts/solar/series.h
        parts/solar/series.hxx
        parts/solar/series.cpp
        parts/solar/prepro.cpp
        parts/solar/prepro.h
        parts/solar/prepro.hxx

        # Study
        header.h
        header.hxx
        header.cpp
        load-options.h
        load-options.cpp
        runtime/runtime.h
        runtime/runtime.hxx
        runtime/runtime.cpp
        runtime.h
        study.h
        study.hxx
        fwd.h
        fwd.cpp
        study.h
        study.cpp
        correlation-updater.hxx
        study.importprepro.cpp
        importlogs.cpp
        load.cpp
        save.cpp
        study.extra.cpp
        version.h
        version.cpp
        cleaner.h
        cleaner/cleaner.cpp cleaner/versions.h
        cleaner/cleaner-v20.cpp
        output.cpp
        output.h
        output.hxx
        layerdata.cpp
        layerdata.h

        # progression
        progression/progression.h
        progression/progression.hxx
        progression/progression.cpp

        # Simulation
        simulation.h
        simulation.cpp

        # Filter
        filter.h
        filter.cpp

        # Sets
        sets.h
        sets.hxx

        # variable selection
        variable-print-info.h
        variable-print-info.cpp
)

source_group("study\\study" FILES ${SRC_STUDY})

set(SRC_ALL
        ${SRC_STDCXX}
        ${SRC_AREAS}
        ${SRC_XCAST}
        ${SRC_STUDY_AREA_UI}
        ${SRC_STUDY_ACTIONS}
        ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_THERMAL}
        ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_AREA}
        ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_LINK}
        ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_CONSTRAINT}
        ${SRC_STUDY_UI}
        ${SRC_STUDY_PARAMETERS}
        ${SRC_STUDY_FINDER}
        ${SRC_STUDY_BINDING_CONSTRAINT}
        ${SRC_STUDY_PART_WIND}
        ${SRC_STUDY_PART_HYDRO}
        ${SRC_STUDY_PART_COMMON}
        ${SRC_STUDY_PART_THERMAL}
        ${SRC_STUDY_PART_RENEWABLE}
        ${SRC_STUDY_PART_SHORT_TERM_STORAGE}
        ${SRC_STUDY_SCENARIO_BUILDER}
        ${SRC_STUDY}
)

add_library(study ${SRC_ALL})
add_library(Antares::study ALIAS study)

target_link_libraries(study
    PUBLIC
        Boost::boost
        yuni-static-core
        Antares::array #xcast
        Antares::date #parameters
        Antares::inifile #parameters
        Antares::stdcxx #enum.hxx
        Antares::utils #area.hxx
        Antares::correlation
        Antares::path
        Antares::mersenne
        Antares::result_writer #study.h
        Antares::object
    PRIVATE
        Antares::exception
)

target_include_directories(study
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)