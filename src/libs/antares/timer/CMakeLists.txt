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

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
