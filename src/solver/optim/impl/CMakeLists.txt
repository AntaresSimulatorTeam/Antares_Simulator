set(PROJ optim_impl)
set(HEADERS
        LinearProblemImpl.h
)
set(SRC_OPTIM
        ${HEADERS}
        LinearProblemImpl.cpp
)
source_group("solver\\optim\\impl" FILES ${SRC_OPTIM})

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
