set(PROJ optimization-options)

set(SRC
    include/antares/optimization-options/options.h
    options.cpp
)

add_library(${PROJ} ${SRC})

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(Antares::${PROJ} ALIAS ${PROJ})

install(DIRECTORY   include/antares
        DESTINATION "include")
