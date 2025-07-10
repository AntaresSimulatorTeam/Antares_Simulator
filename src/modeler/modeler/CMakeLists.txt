#TODO Modeler-lib-2 is a temp name and will be corrected in future commits
add_library(modeler-lib-2 INTERFACE)
add_library(Antares::modeler-lib-2 ALIAS modeler-lib-2)
#target_sources(modeler-lib-2
#        PRIVATE
#        include/antares/solver/modeler/data.h
#)

target_link_libraries(modeler-lib-2 INTERFACE
        Antares::modeler-parameters
        Antares::optim-model-filler
        Antares::linear-problem-api
        # TODO FIXME don't depend on implementations
        Antares::linear-problem-mpsolver-impl
)

target_include_directories(modeler-lib-2 INTERFACE

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
