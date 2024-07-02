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
	antares-core
        Antares::logs
        Antares::inifile
        Antares::config
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)