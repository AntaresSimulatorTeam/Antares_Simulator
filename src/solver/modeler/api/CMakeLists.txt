set(PROJ modeler_api)

set(SRC_API
        include/antares/solver/modeler/api/mipVariable.h
        include/antares/solver/modeler/api/mipSolution.h
        include/antares/solver/modeler/api/mipConstraint.h

        include/antares/solver/modeler/api/hasBounds.h
        include/antares/solver/modeler/api/hasName.h

        include/antares/solver/modeler/api/linearProblem.h
        include/antares/solver/modeler/api/linearProblemData.h
        include/antares/solver/modeler/api/linearProblemFiller.h
        include/antares/solver/modeler/api/linearProblemBuilder.h

        linearProblemData.cpp
		linearProblemBuilder.cpp
)

add_library(${PROJ} ${SRC_API})
add_library(Antares::${PROJ} ALIAS ${PROJ})

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)


target_include_directories(${PROJ}
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
