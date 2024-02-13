set(PROJ locale)
set(HEADERS
        include/antares/locale/locale.h
)
set(SRC_LOCALE
        ${HEADERS}
        locale.cpp)
source_group("locale" FILES ${SRC_LOCALE})

add_library(${PROJ} ${SRC_LOCALE}) #Memory conflict with absl memory when building deps https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/1589
add_library(Antares::locale ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
