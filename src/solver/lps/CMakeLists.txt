set(PROJ lps)
set(HEADERS
        include/antares/solver/lps/LpsFromAntares.h
)
set(SRC_PROJ
        ${HEADERS}
        LpsFromAntares.cpp
)
source_group("solver\\lps" FILES ${SRC_PROJ})

add_library(${PROJ})
target_sources(${PROJ} PRIVATE
        ${SRC_PROJ}
)
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)