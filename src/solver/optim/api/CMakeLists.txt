set(PROJ optim-api)

set(HEADERS
        include/antares/solver/optim/api/mipVariable.h
)
set(SRC_OPTIM
        ${HEADERS}
)

source_group("solver\\optim\\api" FILES ${SRC_OPTIM})

add_library(${PROJ} ${SRC_OPTIM})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_include_directories(${PROJ}
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
