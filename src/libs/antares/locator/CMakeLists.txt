set(PROJ locator)
set(HEADERS
        include/antares/${PROJ}/locator.h
)
set(SRC_LOCATOR
        ${HEADERS}
        locator.cpp
)
source_group("misc\\${PROJ}" FILES ${SRC_LOCATOR})

add_library(${PROJ} ${SRC_LOCATOR})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        study
        PRIVATE
        yuni-static-core
        Antares::resources
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
