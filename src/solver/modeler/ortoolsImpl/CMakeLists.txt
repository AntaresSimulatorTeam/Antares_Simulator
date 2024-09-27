set(PROJ modeler-ortools-impl)

set(HEADERS
        include/antares/solver/modeler/ortoolsImpl/mipVariable.h
        include/antares/solver/modeler/ortoolsImpl/mipSolution.h
        include/antares/solver/modeler/ortoolsImpl/mipConstraint.h

        include/antares/solver/modeler/ortoolsImpl/linearProblem.h
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
        Antares::modeler_api
        Antares::logs
        Antares::solverUtils
        ortools::ortools
)

target_include_directories(${PROJ}
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
