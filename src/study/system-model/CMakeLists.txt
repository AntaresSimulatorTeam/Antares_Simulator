project(SystemModel)

set(SRC_model
        library.cpp
        model.cpp
        component.cpp
        system.cpp
        connection.cpp
        optimConfig.cpp
        portType.cpp
        include/antares/study/system-model/library.h
        include/antares/study/system-model/model.h
        include/antares/study/system-model/parameter.h
        include/antares/study/system-model/valueType.h
        include/antares/study/system-model/variable.h
        include/antares/study/system-model/constraint.h
        include/antares/study/system-model/port.h
        include/antares/study/system-model/portField.h
        include/antares/study/system-model/portFieldDefinition.h
        include/antares/study/system-model/portType.h
        include/antares/study/system-model/component.h
        include/antares/study/system-model/extraOutput.h
        include/antares/study/system-model/system.h
        include/antares/study/system-model/timeAndScenarioType.h
        include/antares/study/system-model/connection.h
        include/antares/study/system-model/objective.h
        include/antares/study/system-model/optimConfig.h
        include/antares/study/system-model/variabilityType.h
)

source_group("SystemModel" FILES ${SRC_model})
# This target is part of a dependency cycle; keep it static in shared builds.
set(_study_system_model_library_type)
if (BUILD_SHARED_LIBS)
    set(_study_system_model_library_type STATIC)
endif ()

add_library(antares-study-system-model
        ${_study_system_model_library_type}
        ${SRC_model})
add_library(Antares::antares-study-system-model ALIAS antares-study-system-model)

target_include_directories(antares-study-system-model
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(antares-study-system-model
        PUBLIC
        Antares::expressions
        PRIVATE
        Antares::exception
)
install(DIRECTORY include/antares
        DESTINATION "include"
)
