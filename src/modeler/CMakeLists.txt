add_subdirectory(loadFiles)
add_subdirectory(modeler)
add_subdirectory(parameters)
add_subdirectory(checks)

OMESSAGE("  :: modeler")

set(exec_name "antares-modeler")

add_executable(antares-modeler
        main.cpp)

set_target_properties(antares-modeler PROPERTIES OUTPUT_NAME ${exec_name})

target_link_libraries(antares-modeler
        PRIVATE
        Antares::modeler-optimisation-container
        Antares::modeler-lib
        Antares::load-modeler-files
        Antares::simu-table-generator
        Antares::logs)

import_std_libs(antares-modeler)
executable_strip(antares-modeler)

copy_dependency(sirius_solver antare s-modeler)
copy_project_shared_lib(simulation-table-writers antares-modeler)

# Install the default variable-names mapping into the data dir. loadAll.cpp reads it
# from this location at runtime (a study may override it with its own
# input/variable-names.yml). GNUInstallDirs defines CMAKE_INSTALL_DATADIR in this
# scope; without it the destination would resolve to the absolute path "/antares".
include(GNUInstallDirs)
install(TARGETS antares-modeler EXPORT antares-modeler DESTINATION bin)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/variable-names.yml"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/antares)

INSTALL(EXPORT antares-modeler
        FILE antares-modelerConfig.cmake
        DESTINATION cmake)
