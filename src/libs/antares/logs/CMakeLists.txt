set(PROJ logs)
set(HEADERS
        ${PROJ}
)
set(SRC_LOGS
        logs.cpp
        cleaner.cpp
        )
source_group("misc\\logs" FILES ${SRC_LOGS})

add_library(${PROJ} ${SRC_LOGS})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
        )

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
