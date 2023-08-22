set(PROJ memory)
set(HEADERS
        include/antares/${PROJ}/memory.h
        include/antares/${PROJ}/memory.hxx
        include/antares/${PROJ}/new_check.hxx
)
set(SRC_MEMORY
        ${HEADERS}
        memory.cpp)
source_group("memory" FILES ${SRC_MEMORY})

add_library(${PROJ} ${SRC_MEMORY})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
        logs
        Antares::sys
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
