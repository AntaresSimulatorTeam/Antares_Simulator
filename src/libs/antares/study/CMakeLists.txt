add_library(series)
add_library(Antares::series ALIAS series)

set(SRC_STUDY_PART_SERIES
        parts/series/include/antares/series/series.h
        parts/series/series.cpp
)
source_group("study\\part\\series" FILES ${SRC_STUDY_PART_SERIES})

target_sources(series PUBLIC ${SRC_STUDY_PART_SERIES})

target_link_libraries(series PUBLIC Antares::array)

target_include_directories(series
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/parts/series
)

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

set(SRC_AREAS
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
        parts/wind/prepro.h
        parts/wind/prepro.hxx
        parts/wind/prepro.cpp
)
source_group("study\\part\\wind" FILES ${SRC_STUDY_PART_WIND})

set(SRC_STUDY_PART_HYDRO
        parts/hydro/container.h
        parts/hydro/container.cpp
        parts/hydro/series.h
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
        parts/load/prepro.cpp
        parts/load/prepro.h
        parts/load/prepro.hxx

        # Solar
        parts/solar/container.h
        parts/solar/container.cpp
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
        Antares::series
    PRIVATE
        Antares::exception
        Antares::benchmarking
)

target_include_directories(study
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../.. #Dirty trick to allow inclusion via <antares/study/X>
        # Make more than just study visible but it's the lesser evil for now
)
