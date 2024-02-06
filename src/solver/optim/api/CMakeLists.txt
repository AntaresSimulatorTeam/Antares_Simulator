set(PROJ optim_api
)
set(HEADERS
        include/antares/optim/api/LinearProblem.h
        include/antares/optim/api/MipSolution.h
        include/antares/optim/api/LinearProblemFiller.h
        include/antares/optim/api/LinearProblemBuilder.h
        include/antares/optim/api/LinearProblemData.h
)
set(SRC_OPTIM
        ${HEADERS}
        LinearProblemBuilder.cpp
)
source_group("solver\\optim\\api" FILES ${SRC_OPTIM})

add_library(${PROJ} ${SRC_OPTIM})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
    ortools::ortools
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

