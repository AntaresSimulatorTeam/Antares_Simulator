set(PROJ logs)
set(HEADERS
        antares/${PROJ}/cleaner.h
        antares/${PROJ}/logs.h
        antares/${PROJ}/logs.hxx
        antares/${PROJ}/hostinfo.h
        antares/${PROJ}/hostname.hxx
)
set(SRC_LOGS
        ${HEADERS}
        logs.cpp
        cleaner.cpp
        hostinfo.cpp
        )
source_group("misc\\logs" FILES ${SRC_LOGS})

add_library(${PROJ} ${SRC_LOGS})
add_library(Antares::${PROJ} ALIAS ${PROJ})

if(WIN32)
        find_library(WSOCK32_LIBRARY wsock32)
        find_library(WS2_32_LIBRARY ws2_32)
        target_link_libraries(${PROJ} PUBLIC wsock32 ws2_32)
endif()
target_link_libraries(${PROJ}
        PUBLIC
        yuni-static-core
        )

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
