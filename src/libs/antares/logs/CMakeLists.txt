set(PROJ logs)
set(HEADERS
        antares/${PROJ}/cleaner.h
        antares/${PROJ}/logs.h
        antares/${PROJ}/logs.hxx
        antares/${PROJ}/hostinfo.h
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

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
        Antares::sys
        )

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
