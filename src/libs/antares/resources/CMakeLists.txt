set(PROJ resources)
set(HEADERS
    include/antares/resources/resources.h
)
set(SRC_RESOURCES
        ${HEADERS}
        resources.cpp)
source_group("resources" FILES ${SRC_RESOURCES})

add_library(${PROJ} ${SRC_RESOURCES}) #Memory conflict with absl memory when building deps https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/1589
add_library(Antares::resources ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        yuni-static-core
        PRIVATE
        Antares::logs
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
