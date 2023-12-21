set(PROJ optimization-options)

add_library(${PROJ} INTERFACE)

target_include_directories(${PROJ}
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(Antares::${PROJ} ALIAS ${PROJ})
