set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_AREA
        handler/antares-study/area/create.h
        handler/antares-study/area/create.hxx
        handler/antares-study/area/create.cpp
        handler/antares-study/area/position.h
        handler/antares-study/area/position.cpp
        handler/antares-study/area/filtering.h
        handler/antares-study/area/filtering.cpp
        handler/antares-study/area/color.h
        handler/antares-study/area/color.cpp
        handler/antares-study/area/nodal-optimization.h
        handler/antares-study/area/nodal-optimization.cpp
        handler/antares-study/area/reserves.h
        handler/antares-study/area/reserves.cpp
        handler/antares-study/area/misc-gen.h
        handler/antares-study/area/misc-gen.cpp
        handler/antares-study/area/timeseries.h
        handler/antares-study/area/timeseries.cpp
        handler/antares-study/area/ts-generator.h
        handler/antares-study/area/ts-generator.cpp
        handler/antares-study/area/ts-node.h
        handler/antares-study/area/ts-node.hxx
        handler/antares-study/area/ts-node.cpp
        handler/antares-study/area/correlation.h
        handler/antares-study/area/correlation.cpp
        handler/antares-study/area/correlation-post.h
        handler/antares-study/area/correlation-post.hxx
        handler/antares-study/area/correlation-post.cpp

        handler/antares-study/area/allocation-hydro.h
        handler/antares-study/area/allocation-hydro.cpp
        handler/antares-study/area/allocation-hydro-post.h
        handler/antares-study/area/allocation-hydro-post.hxx
        handler/antares-study/area/allocation-hydro-post.cpp)
source_group("study\\actions\\handler\\antares study\\area" FILES ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_AREA})

set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_LINK
        handler/antares-study/link/create.h
        handler/antares-study/link/create.hxx
        handler/antares-study/link/create.cpp
        handler/antares-study/link/ntc.h
        handler/antares-study/link/ntc.cpp
        handler/antares-study/link/hurdles-cost.h
        handler/antares-study/link/hurdles-cost.cpp
        handler/antares-study/link/impedances.h
        handler/antares-study/link/impedances.cpp
        handler/antares-study/link/comments.h
        handler/antares-study/link/comments.hxx
        handler/antares-study/link/comments.cpp
        handler/antares-study/link/copper-plate.h
        handler/antares-study/link/copper-plate.cpp
        handler/antares-study/link/filtering.h
        handler/antares-study/link/filtering.cpp
        handler/antares-study/link/unload-data.h
        handler/antares-study/link/unload-data.hxx
        handler/antares-study/link/unload-data.cpp)
source_group("study\\actions\\handler\\antares study\\link" FILES ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_LINK})

set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_THERMAL
        handler/antares-study/thermal-cluster/create.h
        handler/antares-study/thermal-cluster/create.hxx
        handler/antares-study/thermal-cluster/create.cpp
        handler/antares-study/thermal-cluster/root-node.h
        handler/antares-study/thermal-cluster/root-node.hxx
        handler/antares-study/thermal-cluster/root-node.cpp
        handler/antares-study/thermal-cluster/common.h
        handler/antares-study/thermal-cluster/common.cpp
)
source_group("study\\action\\handler\\sntares study\\thermal cluster" FILES ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_THERMAL})


set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_CONSTRAINT
        handler/antares-study/constraint/create.h
        handler/antares-study/constraint/create.hxx
        handler/antares-study/constraint/create.cpp
        handler/antares-study/constraint/comments.h
        handler/antares-study/constraint/comments.hxx
        handler/antares-study/constraint/comments.cpp
        handler/antares-study/constraint/data.h
        handler/antares-study/constraint/data.hxx
        handler/antares-study/constraint/data.cpp
        handler/antares-study/constraint/weights.h
        handler/antares-study/constraint/weights.cpp
        handler/antares-study/constraint/offsets.h
        handler/antares-study/constraint/offsets.cpp
        handler/antares-study/constraint/enabled.h
        handler/antares-study/constraint/enabled.hxx
        handler/antares-study/constraint/enabled.cpp
)
source_group("study\\action\\handler\\antares study\\constraint" FILES ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_CONSTRAINT})

set(SRC_STUDY_ACTIONS
        fwd.h
        action.h
        action.hxx
        action.cpp
        text.h
        text.hxx
        settings/suffix-for-area-names.h
        settings/suffix-for-area-names.hxx
        settings/suffix-for-area-names.cpp
        settings/suffix-for-cluster-names.h
        settings/suffix-for-cluster-names.hxx
        settings/suffix-for-cluster-names.cpp
        settings/suffix-for-constraint-names.h
        settings/suffix-for-constraint-names.hxx
        settings/suffix-for-constraint-names.cpp
        settings/decal-area-position.h
        settings/decal-area-position.hxx
        settings/decal-area-position.cpp
        behavior.h
        behavior.cpp
        state.h
        state.cpp
        context.h
        context.cpp)
source_group("study\\actions" FILES ${SRC_STUDY_ACTIONS})


set(SRC_HANDLER
        ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_THERMAL}
        ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_AREA}
        ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_LINK}
        ${SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_CONSTRAINT}
        ${SRC_STUDY_ACTIONS})

add_library(antares-action ${SRC_HANDLER})

target_link_libraries(antares-action
    PUBLIC
        yuni-static-core
        Antares::result_writer #study.h
        Antares::date #parameters
        Antares::inifile #parameters
        Antares::object
        Antares::study
)

add_library(Antares::action ALIAS antares-action)
