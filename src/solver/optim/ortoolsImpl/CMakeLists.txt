set(PROJ optim-ortools-impl)

set(HEADERS
        include/antares/solver/optim/ortoolsImpl/mipVariable.h
        include/antares/solver/optim/ortoolsImpl/mipSolution.h
        include/antares/solver/optim/ortoolsImpl/mipObjective.h
)
set(SRC_ORTOOLS_IMPL
        ${HEADERS}
        mipVariable.cpp
        mipSolution.cpp
        mipObjective.cpp
)

source_group("solver\\optim\\api" FILES ${SRC_ORTOOLS_IMPL})

add_library(${PROJ} ${SRC_ORTOOLS_IMPL})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        Antares::optim_api
        ortools::ortools
)

target_include_directories(${PROJ}
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
