set(SOURCES
    legacyToYaml.cpp
    generateSystemForView.cpp
    include/antares/view-builder/legacyToYaml.h
)

add_library(view-builder ${SOURCES})
add_library(Antares::view-builder ALIAS view-builder)

target_include_directories(view-builder
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/solver/optimisation/include>
)

target_link_libraries(view-builder
    PUBLIC
    Antares::antares-core
    yaml-cpp::yaml-cpp
)
