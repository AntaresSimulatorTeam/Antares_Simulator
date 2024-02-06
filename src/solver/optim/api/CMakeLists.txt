set(PROJ optim_api
)
set(HEADERS
        LinearProblem.h
        MipSolution.h
        LinearProblemFiller.h
        LinearProblemBuilder.h
)
set(SRC_OPTIM
        ${HEADERS}
        LinearProblemBuilder.cpp
)
source_group("solver\\optim\\api" FILES ${SRC_OPTIM})

add_library(${PROJ} ${SRC_OPTIM})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
  PRIVATE
  ortools::ortools
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

