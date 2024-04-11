set(PROJ timer)
set(HEADERS
        include/antares/${PROJ}/timer.h
)
set(SRC_TIMER
        ${HEADERS}
        timer.cpp
)
source_group("misc\\timer" FILES ${SRC_TIMER})

add_library(${PROJ} ${SRC_TIMER})
add_library(Antares::${PROJ} ALIAS ${PROJ})

if (WIN32)
    find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
    target_link_libraries(${PROJ} PUBLIC wsock32 ws2_32)
endif ()
target_link_libraries(${PROJ}
        PUBLIC
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
