project(SystemModel)

set(SRC_model
        library.cpp
        model.cpp
        component.cpp
        system.cpp
        include/antares/study/system-model/library.h
        include/antares/study/system-model/model.h
        include/antares/study/system-model/parameter.h
        include/antares/study/system-model/valueType.h
        include/antares/study/system-model/variable.h
        include/antares/study/system-model/constraint.h
        include/antares/study/system-model/expression.h
        include/antares/study/system-model/port.h
        include/antares/study/system-model/portField.h
        include/antares/study/system-model/portFieldDefinition.h
        include/antares/study/system-model/portType.h
        include/antares/study/system-model/component.h
        include/antares/study/system-model/system.h
)

source_group("SystemModel" FILES ${SRC_model})
add_library(antares-study-system-model
        ${SRC_model})
add_library(Antares::antares-study-system-model ALIAS antares-study-system-model)

target_include_directories(antares-study-system-model
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(antares-study-system-model
        PUBLIC
	Antares::solver-expressions
)
install(DIRECTORY include/antares
        DESTINATION "include"
)
