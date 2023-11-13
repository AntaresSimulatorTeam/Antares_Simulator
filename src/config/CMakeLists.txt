set(PROJ config)
set(HEADERS
        include/antares/config/config.h
)
set(SRC_CONFIG
        ${HEADERS}
        config.cpp
)
source_group("config" FILES ${SRC_CONFIG})

add_library(${PROJ} ${SRC_CONFIG}) #Memory conflict with absl memory when building deps https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/1589
add_library(Antares::config ALIAS ${PROJ})

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
