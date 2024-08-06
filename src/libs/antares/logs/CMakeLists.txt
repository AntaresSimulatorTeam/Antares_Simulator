set(PROJ logs)
set(HEADERS
        include/antares/${PROJ}/logs.h
        include/antares/${PROJ}/hostinfo.h
        include/antares/${PROJ}/hostname.hxx
)
set(SRC_LOGS
        ${HEADERS}
        logs.cpp
        hostinfo.cpp
)
source_group("misc\\logs" FILES ${SRC_LOGS})

add_library(${PROJ} ${SRC_LOGS})
add_library(Antares::${PROJ} ALIAS ${PROJ})

if (WIN32)
    find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
    target_link_libraries(${PROJ} PUBLIC wsock32 ws2_32)
endif ()
target_link_libraries(${PROJ}
        PUBLIC
        yuni-static-core
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)