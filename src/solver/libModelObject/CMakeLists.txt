project(LibObjectModel)

set(SRC_model
    model.cpp

    include/antares/solver/libObjectModel/library.h
    include/antares/solver/libObjectModel/model.h
    include/antares/solver/libObjectModel/parameter.h
    include/antares/solver/libObjectModel/valueType.h
    include/antares/solver/libObjectModel/variable.h
    include/antares/solver/libObjectModel/constraint.h
    include/antares/solver/libObjectModel/expression.h
    include/antares/solver/libObjectModel/port.h
    include/antares/solver/libObjectModel/portField.h
    include/antares/solver/libObjectModel/portFieldDefinition.h
    include/antares/solver/libObjectModel/portType.h
)

source_group("libObjectModel" FILES ${SRC_model})
add_library(antares-solver-libObjectModel
        ${SRC_model})

target_include_directories(antares-solver-libObjectModel
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(antares-solver-libObjectModel
        PUBLIC
)
install(DIRECTORY include/antares
        DESTINATION "include"
)
