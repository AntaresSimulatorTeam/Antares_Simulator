set(PROJ linear-problem-api)

set(SRC_API
        include/antares/optimisation/linear-problem-api/mipVariable.h
        include/antares/optimisation/linear-problem-api/mipSolution.h
        include/antares/optimisation/linear-problem-api/mipConstraint.h
        include/antares/optimisation/linear-problem-api/hasBounds.h
        include/antares/optimisation/linear-problem-api/hasName.h
        include/antares/optimisation/linear-problem-api/ILinearProblemData.h
        include/antares/optimisation/linear-problem-api/linearProblem.h
        include/antares/optimisation/linear-problem-api/linearProblemFiller.h
        include/antares/optimisation/linear-problem-api/linearProblemBuilder.h
        linearProblemBuilder.cpp
)

add_library(${PROJ} ${SRC_API})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)


target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
