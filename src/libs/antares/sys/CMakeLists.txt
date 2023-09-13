set(PROJ sys)
set(HEADERS
        include/antares/${PROJ}/appdata.h
        include/antares/${PROJ}/policy.h
)
set(SRC_PROJ
        ${HEADERS}
        policy.cpp
        appdata.cpp
)
source_group("memory" FILES ${SRC_PROJ})

add_library(${PROJ} ${SRC_PROJ})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        yuni-static-core
        PRIVATE
        Antares::logs
        Antares::inifile
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs
)
