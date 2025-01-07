add_subdirectory(api)
add_subdirectory(ortoolsImpl)
add_subdirectory(loadFiles)
add_subdirectory(parameters)

OMESSAGE("  :: modeler")

set(exec_name "antares-modeler")

add_library(modeler-lib INTERFACE
        ${SRCS}
)

add_executable(antares-modeler
        main.cpp
        ${SRCS}
)

set_target_properties(antares-modeler PROPERTIES OUTPUT_NAME ${exec_name})

target_link_libraries(modeler-lib
        INTERFACE
        Antares::loadModelerFiles
        Antares::modelerParameters
	Antares::optim-model-filler
	Antares::modeler_api
	# TODO FIXME don't depend on implementations
	Antares::modeler-ortools-impl
)

target_link_libraries(antares-modeler
        PRIVATE
        modeler-lib
)

import_std_libs(antares-modeler)
executable_strip(antares-modeler)

copy_dependency(sirius_solver antares-modeler)

install(TARGETS antares-modeler EXPORT antares-modeler DESTINATION bin)

INSTALL(EXPORT antares-modeler
        FILE antares-modelerConfig.cmake
        DESTINATION cmake
)
