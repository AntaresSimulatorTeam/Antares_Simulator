project(Model)

set(SRC_model
    model.cpp

    include/antares/solver/model/model.h
    include/antares/solver/model/parameter.h
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
