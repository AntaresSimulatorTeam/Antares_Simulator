project(constraints-builder)
#
# Time-Series Generators
#
set(SRC_CBUILDER
        cbuilder.cpp
        include/antares/solver/constraints-builder/cbuilder.h
        create.cpp
        include/antares/solver/constraints-builder/grid.h
        include/antares/solver/constraints-builder/grid.hxx
        load.cpp
)
source_group("constraints-builder" FILES ${SRC_CBUILDER})


#
# --- Library VARIABLES ---
#
add_library(antares-solver-constraints-builder
        ${SRC_CBUILDER})


target_link_libraries(antares-solver-constraints-builder PRIVATE antares-core
        PUBLIC
        Antares::study
)

target_include_directories(antares-solver-constraints-builder
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)