set(PROJ optim_standard
)
set(HEADERS
)
set(SRC_OPTIM
        ${HEADERS}
)
source_group("solver\\optim\\standard" FILES ${SRC_OPTIM})

add_library(${PROJ} ${SRC_OPTIM})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}

)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${PROJ} PROPERTIES LINKER_LANGUAGE CXX)

