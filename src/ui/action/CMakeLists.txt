set(SRC_STUDY_ACTIONS_HANDLER_ANTARES_STUDY_AREA
        include/action/handler/antares-study/area/create.h
        include/action/handler/antares-study/area/create.hxx
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
        include/action/handler/antares-study/link/create.h
        include/action/handler/antares-study/link/create.hxx
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
        include/action/handler/antares-study/constraint/create.h
        include/action/handler/antares-study/constraint/create.hxx
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
        include/action/fwd.h
        include/action/action.h
        include/action/action.hxx
        action.cpp
        include/action/text.h
        include/action/text.hxx
        include/action/settings/suffix-for-area-names.h
        include/action/settings/suffix-for-area-names.hxx
        settings/suffix-for-area-names.cpp
        include/action/settings/suffix-for-cluster-names.h
        include/action/settings/suffix-for-cluster-names.hxx
        settings/suffix-for-cluster-names.cpp
        include/action/settings/suffix-for-constraint-names.h
        include/action/settings/suffix-for-constraint-names.hxx
        settings/suffix-for-constraint-names.cpp
        include/action/settings/decal-area-position.h
        include/action/settings/decal-area-position.hxx
        settings/decal-area-position.cpp
        include/action/behavior.h
        behavior.cpp
        include/action/state.h
        state.cpp
        include/action/context.h
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

target_include_directories(antares-action
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(Antares::action ALIAS antares-action)
