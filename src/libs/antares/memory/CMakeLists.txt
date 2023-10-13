set(PROJ AntaresMemory)
set(HEADERS
        include/antares/memory/memory.h
        include/antares/memory/memory.hxx
        include/antares/memory/new_check.hxx
)
set(SRC_MEMORY
        ${HEADERS}
        memory.cpp)
source_group("memory" FILES ${SRC_MEMORY})

add_library(${PROJ} ${SRC_MEMORY}) #Memory conflict with absl memory when building deps https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/1589
add_library(Antares::memory ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        yuni-static-core
        logs
        PRIVATE
        Antares::sys
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
