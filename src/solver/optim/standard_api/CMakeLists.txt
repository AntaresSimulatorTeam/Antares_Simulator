set(PROJ optim_standard_api
)
set(HEADERS
        include/antares/optim/standard_api/Component.h
        include/antares/optim/standard_api/ComponentFiller.h
        include/antares/optim/standard_api/ConnectableLinearProblemFiller.h
        include/antares/optim/standard_api/PortConnection.h
)
set(SRC_OPTIM
        ${HEADERS}
        ComponentFiller.cpp
)
source_group("solver\\optim\\standard_api" FILES ${SRC_OPTIM})

add_library(${PROJ} ${SRC_OPTIM})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        Antares::optim_api
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
