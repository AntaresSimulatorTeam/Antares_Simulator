set(PROJ sys)
set(HEADERS
        include/antares/${PROJ}/appdata.h
        include/antares/${PROJ}/policy.h
        include/antares/${PROJ}/hostname.hxx
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
        PRIVATE
        yuni-static-core
        Antares::logs
        Antares::inifile
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs
)
