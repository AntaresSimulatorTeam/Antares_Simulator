set(PROJ linear-problem-api)

set(SRC_API
        include/antares/optimisation/linear-problem-api/ILinearProblemData.h
        include/antares/optimisation/linear-problem-api/IScenario.h
        include/antares/optimisation/linear-problem-api/hasBounds.h
        include/antares/optimisation/linear-problem-api/hasName.h
        include/antares/optimisation/linear-problem-api/hasStatus.h
        include/antares/optimisation/linear-problem-api/linearProblem.h
        include/antares/optimisation/linear-problem-api/linearProblemBuilder.h
        include/antares/optimisation/linear-problem-api/linearProblemFiller.h
        include/antares/optimisation/linear-problem-api/mipConstraint.h
        include/antares/optimisation/linear-problem-api/mipSolution.h
        include/antares/optimisation/linear-problem-api/mipVariable.h
        linearProblemBuilder.cpp
)

add_library(${PROJ} ${SRC_API})
add_library(Antares::${PROJ} ALIAS ${PROJ})

# StructuredLinearProblem - partial implementation of ILinearProblem (store problem, no resolution)
add_library(structured-linear-problem
        StructuredLinearProblem.cpp
        include/antares/optimisation/linear-problem-api/StructuredLinearProblem.h
)
add_library(Antares::structured-linear-problem ALIAS structured-linear-problem)

target_include_directories(structured-linear-problem
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(structured-linear-problem
        PUBLIC
        Antares::linear-problem-api
)

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

install(DIRECTORY include/antares
        DESTINATION "include"
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
