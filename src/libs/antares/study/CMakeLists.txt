set(SRC_STUDY_SCENARIO_BUILDER
        include/antares/study/scenario-builder/scBuilderDataInterface.h
        include/antares/study/scenario-builder/TSnumberData.h
        scenario-builder/TSnumberData.cpp
        include/antares/study/scenario-builder/hydroLevelsData.h
        scenario-builder/hydroLevelsData.cpp
        include/antares/study/scenario-builder/rules.h
        include/antares/study/scenario-builder/rules.hxx
        scenario-builder/rules.cpp
        include/antares/study/scenario-builder/sets.h
        include/antares/study/scenario-builder/sets.hxx
        scenario-builder/sets.cpp
        include/antares/study/scenario-builder/updater.hxx
        include/antares/study/scenario-builder/scBuilderUtils.h
        scenario-builder/scBuilderUtils.cpp
        scenario-builder/BindingConstraintsTSNumbersData.cpp
        scenario-builder/NTCTSNumberData.cpp
        include/antares/study/scenario-builder/NTCTSNumberData.h
        include/antares/study/scenario-builder/BindingConstraintsTSNumbersData.h
        include/antares/study/scenario-builder/RenewableTSNumberData.h
        scenario-builder/RenewableTSNumberData.cpp
        scenario-builder/ThermalTSNumberData.cpp
        include/antares/study/scenario-builder/ThermalTSNumberData.h
        include/antares/study/scenario-builder/HydroTSNumberData.h
        scenario-builder/HydroTSNumberData.cpp
        scenario-builder/SolarTSNumberData.cpp
        include/antares/study/scenario-builder/solarTSNumberData.h
        include/antares/study/scenario-builder/WindTSNumberData.h
        scenario-builder/WindTSNumberData.cpp
        include/antares/study/scenario-builder/LoadTSNumberData.h
        scenario-builder/LoadTSNumberData.cpp
        include/antares/study/scenario-builder/applyToMatrix.hxx
)
source_group("study\\scenario builder" FILES ${SRC_MATRIX})

set(SRC_AREAS
        include/antares/study/area/constants.h
        include/antares/study/area/area.h
        include/antares/study/area/area.hxx
        include/antares/study/area/scratchpad.h
        area/scratchpad.cpp
        area/area.cpp
        area/links.cpp
        include/antares/study/area/links.h
        area/store-timeseries-numbers.cpp
        include/antares/study/area/store-timeseries-numbers.h
        include/antares/study/area/store-timeseries-numbers.hxx
        area/list.cpp
)
source_group("study\\area" FILES ${SRC_AREAS})


set(SRC_STUDY_AREA_UI
        include/antares/study/area/ui.h
        area/ui.cpp
)
source_group("study\\area\\ui" FILES ${SRC_STUDY_AREA_UI})


set(SRC_STUDY_PART_COMMON
        parts/common/cluster.cpp
        include/antares/study/parts/common/cluster.h
        include/antares/study/parts/common/cluster_list.h
        parts/common/cluster_list.cpp
)
source_group("study\\part\\common" FILES ${SRC_STUDY_PART_COMMON})

set(SRC_STUDY_PART_THERMAL
        include/antares/study/parts/thermal/container.h
        parts/thermal/container.cpp
        include/antares/study/parts/thermal/ecoInput.h
        parts/thermal/ecoInput.cpp
        include/antares/study/parts/thermal/cluster.h
        include/antares/study/parts/thermal/cost_provider.h
        include/antares/study/parts/thermal/cluster.hxx
        parts/thermal/cluster.cpp
        parts/thermal/scenarized_cost_provider.cpp
        parts/thermal/constant_cost_provider.cpp
        include/antares/study/parts/thermal/cluster_list.h
        parts/thermal/cluster_list.cpp
        include/antares/study/parts/thermal/pollutant.h
        parts/thermal/pollutant.cpp
)
source_group("study\\part\\thermal" FILES ${SRC_STUDY_PART_THERMAL})

set(SRC_STUDY_PART_RENEWABLE
        include/antares/study/parts/renewable/container.h
        include/antares/study/parts/renewable/container.hxx
        parts/renewable/container.cpp
        include/antares/study/parts/renewable/cluster.h
        parts/renewable/cluster.cpp
        include/antares/study/parts/renewable/cluster_list.h
        parts/renewable/cluster_list.cpp
)
source_group("study\\part\\renewable" FILES ${SRC_STUDY_PART_RENEWABLE})

set(SRC_STUDY_PART_SHORT_TERM_STORAGE
        include/antares/study/parts/short-term-storage/container.h
        parts/short-term-storage/container.cpp
        include/antares/study/parts/short-term-storage/properties.h
        parts/short-term-storage/properties.cpp
        parts/short-term-storage/series.cpp
        include/antares/study/parts/short-term-storage/series.h
        include/antares/study/parts/short-term-storage/cluster.h
        include/antares/study/parts/short-term-storage/AdditionalConstraint.h
        parts/short-term-storage/cluster.cpp
        parts/short-term-storage/AdditionalConstraint.cpp
)
source_group("study\\part\\short-term-storage" FILES ${SRC_STUDY_PART_SHORT_TERM_SOTRAGE})

set(SRC_STUDY_PART_WIND
        include/antares/study/parts/wind/container.h
        parts/wind/container.cpp
        include/antares/study/parts/wind/prepro.h
        include/antares/study/parts/wind/prepro.hxx
        parts/wind/prepro.cpp
)
source_group("study\\part\\wind" FILES ${SRC_STUDY_PART_WIND})

set(SRC_STUDY_PART_HYDRO
        include/antares/study/parts/hydro/container.h
        parts/hydro/container.cpp
        include/antares/study/parts/hydro/series.h
        parts/hydro/series.cpp
        include/antares/study/parts/hydro/prepro.h
        parts/hydro/prepro.cpp
        include/antares/study/parts/hydro/allocation.h
        include/antares/study/parts/hydro/allocation.hxx
        parts/hydro/allocation.cpp
        include/antares/study/parts/hydro/hydromaxtimeseriesreader.h
        parts/hydro/hydromaxtimeseriesreader.cpp
)
source_group("study\\part\\hydro" FILES ${SRC_STUDY_PART_HYDRO})

set(SRC_STUDY_UI
        include/antares/study/ui-runtimeinfos.h
        ui-runtimeinfos.cpp
)
source_group(study\\UI FILES ${SRC_STUDY_UI})

set(SRC_STUDY_PARAMETERS
        include/antares/study/parameters.h
        parameters.cpp
        include/antares/study/parameters/adq-patch-params.h
        parameters/adq-patch-params.cpp
        UnfeasibleProblemBehavior.cpp
        # TODO JMK : should be in a target_include_directories : but need to remove all .h from all SRC
        include/antares/study/UnfeasibleProblemBehavior.hpp
)
source_group("study\\general data" FILES ${SRC_STUDY_PARAMETERS})


set(SRC_STUDY_FINDER
        finder/finder.cpp
        include/antares/study/finder/finder.h
)
source_group("study\\finder" FILES ${SRC_STUDY_FINDER})


set(SRC_STUDY_BINDING_CONSTRAINT
        include/antares/study/constraint.h
        include/antares/study/binding_constraint/BindingConstraint.h
        include/antares/study/binding_constraint/BindingConstraint.hxx
        binding_constraint/BindingConstraint.cpp
        include/antares/study/binding_constraint/BindingConstraintsRepository.h
        include/antares/study/binding_constraint/BindingConstraintsRepository.h
        include/antares/study/binding_constraint/BindingConstraintsRepository.hxx
        binding_constraint/BindingConstraintsRepository.cpp
        binding_constraint/BindingConstraintLoader.cpp
        include/antares/study/binding_constraint/BindingConstraintLoader.h
        binding_constraint/BindingConstraintSaver.cpp
        include/antares/study/binding_constraint/BindingConstraintSaver.h
        include/antares/study/binding_constraint/EnvForLoading.h
        include/antares/study/binding_constraint/BindingConstraintStructures.h
        binding_constraint/BindingConstraintGroup.cpp
        include/antares/study/binding_constraint/BindingConstraintGroup.h
        binding_constraint/BindingConstraintGroupRepository.cpp
        include/antares/study/binding_constraint/BindingConstraintGroupRepository.h
)

source_group("study\\constraint" FILES ${SRC_STUDY_BINDING_CONSTRAINT})


set(SRC_XCAST
        include/antares/study/xcast.h
        include/antares/study/xcast/xcast.h
        xcast/xcast.cpp
)
source_group("study\\xcast" FILES ${SRC_XCAST})

set(SRC_STUDY
        # all
        include/antares/study/parts/parts.h

        # Load
        include/antares/study/parts/load/container.h
        parts/load/container.cpp
        parts/load/prepro.cpp
        include/antares/study/parts/load/prepro.h
        include/antares/study/parts/load/prepro.hxx

        # Solar
        include/antares/study/parts/solar/container.h
        parts/solar/container.cpp
        parts/solar/prepro.cpp
        include/antares/study/parts/solar/prepro.h
        include/antares/study/parts/solar/prepro.hxx

        # Study
        include/antares/study/header.h
        header.cpp
        include/antares/study/load-options.h
        load-options.cpp
        include/antares/study/runtime/runtime.h
        runtime/runtime.cpp
        include/antares/study/runtime.h
        include/antares/study/study.h
        include/antares/study/study.hxx
        include/antares/study/fwd.h
        fwd.cpp
        include/antares/study/study.h
        study.cpp
        include/antares/study/correlation-updater.hxx
        study.importprepro.cpp
        importlogs.cpp
        load.cpp
        save.cpp
        study.extra.cpp
        include/antares/study/version.h
        version.cpp
        include/antares/study/cleaner.h
        cleaner/cleaner.cpp
        include/antares/study/cleaner/versions.h
        cleaner/cleaner-v20.cpp
        output.cpp
        include/antares/study/output.h
        include/antares/study/output.hxx
        layerdata.cpp
        include/antares/study/layerdata.h

        # progression
        include/antares/study/progression/progression.h
        include/antares/study/progression/progression.hxx
        progression/progression.cpp

        # Simulation
        include/antares/study/simulation.h
        simulation.cpp

        # Filter
        include/antares/study/filter.h
        filter.cpp

        # Sets
        include/antares/study/sets.h
        area/sets.cpp

        # variable selection
        include/antares/study/variable-print-info.h
        variable-print-info.cpp
)

source_group("study\\study" FILES ${SRC_STUDY})

set(SRC_ALL
        ${SRC_STDCXX}
        ${SRC_AREAS}
        ${SRC_XCAST}
        ${SRC_STUDY_AREA_UI}
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
        yuni-static-core
        Antares::date #parameters
        Antares::inifile #parameters
        Antares::stdcxx #enum.hxx
        Antares::utils #area.hxx
        Antares::solverUtils #sim_structure_probleme_economique.h -> optimization_statistics.h
        Antares::correlation
        Antares::path
        Antares::mersenne
        Antares::result_writer #study.h
        Antares::series
        Antares::array
        antares-core
        antares-solver-simulation
        Antares::hydro
        PRIVATE
        Antares::exception
        Antares::benchmarking
        antares-solver-variable
)

target_include_directories(study
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        # Make more than just study visible but it's the lesser evil for now
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
