set(PROJ optim_api)

set(SRC_API
        include/antares/solver/optim/api/mipVariable.h
        include/antares/solver/optim/api/mipObjective.h
        include/antares/solver/optim/api/mipSolution.h
        include/antares/solver/optim/api/mipConstraint.h
        include/antares/solver/optim/api/hasBounds.h

        include/antares/solver/optim/api/linearProblem.h
        include/antares/solver/optim/api/linearProblemData.h

        linearProblemData.cpp
)

add_library(${PROJ} ${SRC_API})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJ}
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
