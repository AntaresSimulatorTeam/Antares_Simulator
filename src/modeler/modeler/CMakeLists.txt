add_library(modeler-lib)
add_library(Antares::modeler-lib ALIAS modeler-lib)
target_sources(modeler-lib
        PRIVATE
        Modeler.cpp
        VariableNameMapper.cpp
        include/antares/solver/modeler/Modeler.h
        include/antares/solver/modeler/ILoader.h
        include/antares/solver/modeler/ModelerData.h
        include/antares/solver/modeler/VariableNameMapper.h
)

target_link_libraries(modeler-lib
        PUBLIC
        Antares::modeler-parameters
        Antares::optim-model-filler
        Antares::linear-problem-api
        Antares::structured-linear-problem
        # TODO FIXME don't depend on implementations
        Antares::linear-problem-mpsolver-impl
        Antares::modeler-optimisation-container
        PRIVATE
        Antares::io
        yaml-cpp::yaml-cpp
)

target_include_directories(modeler-lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
