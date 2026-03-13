set(PROJ modeler-optimisation-container)

set(SRC-modeler-optimisation-container
        include/antares/
        include/antares/modeler-optimisation-container/EvaluationContext.h
        include/antares/modeler-optimisation-container/OptimEntityContainer.h
        include/antares/modeler-optimisation-container/scenarioGroupRepo.h
        scenarioGroupRepo.cpp
        scenarioGroupRepoExceptions.cpp
        EvaluationContext.cpp
        OptimEntityContainer.cpp

)

# This target is part of a dependency cycle; keep it static in shared builds.
set(_modeler_optimisation_container_library_type)
if (BUILD_SHARED_LIBS)
    set(_modeler_optimisation_container_library_type STATIC)
endif ()

add_library(${PROJ} ${_modeler_optimisation_container_library_type} ${SRC-modeler-optimisation-container})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJ}
        PRIVATE
        Antares::antares-study-system-model
        Antares::linear-problem-api
        Antares::exception
)

install(DIRECTORY include/antares
        DESTINATION "include"
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
