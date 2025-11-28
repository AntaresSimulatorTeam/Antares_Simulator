set(PROJ modeler-optimisation-container)

set(SRC-modeler-optimisation-container
        include/antares/
        include/antares/modeler-optimisation-container/EvaluationContext.h
        include/antares/modeler-optimisation-container/OptimEntityContainer.h
        include/antares/modeler-optimisation-container/VariabilityType.h
        include/antares/modeler-optimisation-container/scenarioGroupRepo.h
        scenarioGroupRepo.cpp
        scenarioGroupRepoExceptions.cpp
        EvaluationContext.cpp
        OptimEntityContainer.cpp

)

add_library(${PROJ} ${SRC-modeler-optimisation-container})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJ}
        PRIVATE
        Antares::antares-study-system-model
        Antares::linear-problem-api
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
