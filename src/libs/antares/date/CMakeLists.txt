set(PROJ date)
set(HEADERS
        include/antares/${PROJ}/date.h
)
set(SRC_DATE
        ${HEADERS}
        date.cpp
)
source_group("misc\\${PROJ}" FILES ${SRC_DATE})

add_library(${PROJ} ${SRC_DATE})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
        logs
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
