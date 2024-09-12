project(Model)

set(SRC_model
    model.cpp

    include/antares/solver/model/library.h
    include/antares/solver/model/model.h
    include/antares/solver/model/parameter.h
    include/antares/solver/model/valueType.h
    include/antares/solver/model/variable.h
    include/antares/solver/model/constraint.h
    include/antares/solver/model/expression.h
    include/antares/solver/model/port.h
    include/antares/solver/model/portType.h
)

source_group("model" FILES ${SRC_model})
add_library(antares-solver-model
        ${SRC_model})

target_include_directories(antares-solver-model
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(antares-solver-model
        PUBLIC
        Antares::logs
)
install(DIRECTORY include/antares
        DESTINATION "include"
)
