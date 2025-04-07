set(PROJ linear-problem-mpsolver-impl)

set(HEADERS
        include/antares/optimisation/linear-problem-mpsolver-impl/mipVariable.h
        include/antares/optimisation/linear-problem-mpsolver-impl/mipSolution.h
        include/antares/optimisation/linear-problem-mpsolver-impl/mipConstraint.h
        include/antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h
)
set(SRC_ORTOOLS_IMPL
        ${HEADERS}
        mipVariable.cpp
        mipSolution.cpp
        mipConstraint.cpp

        linearProblem.cpp
)

source_group("solver\\modeler\\api" FILES ${SRC_ORTOOLS_IMPL})

add_library(${PROJ} ${SRC_ORTOOLS_IMPL})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        Antares::linear-problem-api
        Antares::logs
        Antares::solverUtils
		# Until issue here is resolved, link the whole archive: https://github.com/google/or-tools/discussions/4538
        # TODO: keep an eye out for bug resolution in future versions of ortools (> 9.12)
        $<LINK_LIBRARY:WHOLE_ARCHIVE,ortools::ortools>
)

target_include_directories(${PROJ}
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
