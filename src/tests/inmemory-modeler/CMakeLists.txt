add_library(inmemory-modeler)
add_library(Antares::tests::inmemory-modeler ALIAS inmemory-modeler)
target_sources(inmemory-modeler
        PRIVATE
    inmemory-modeler.cpp
    include/inmemory-modeler.h
)

target_include_directories(inmemory-modeler
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(inmemory-modeler
        PRIVATE
        antares-study-system-model
        Antares::linear-problem-mpsolver-impl
        Antares::optim-model-filler
        Antares::linear-problem-data-impl
        Antares::expressions
)

set_target_properties(inmemory-modeler PROPERTIES FOLDER Unit-tests)