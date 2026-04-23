add_subdirectory(loadFiles)
add_subdirectory(fileWriter)
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
        Antares::write-modeler-files
        Antares::logs)

import_std_libs(antares-modeler)
executable_strip(antares-modeler)

copy_dependency(sirius_solver antares-modeler)
copy_project_shared_lib(parquet_writer antares-modeler)

install(TARGETS antares-modeler EXPORT antares-modeler DESTINATION bin)

INSTALL(EXPORT antares-modeler
        FILE antares-modelerConfig.cmake
        DESTINATION cmake)
