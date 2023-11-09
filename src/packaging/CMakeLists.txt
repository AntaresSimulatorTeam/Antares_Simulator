message("CCC")
find_package(minizip)
message(minizip_FOUND ${minizip_FOUND})

# Get all properties that cmake supports
execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)

# Convert command output into a CMake list
STRING(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
STRING(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
# Fix https://stackoverflow.com/questions/32197663/how-can-i-remove-the-the-location-property-may-not-be-read-from-target-error-i
list(FILTER CMAKE_PROPERTY_LIST EXCLUDE REGEX "^LOCATION$|^LOCATION_|_LOCATION$")
# For some reason, "TYPE" shows up twice - others might too?
list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)

# build whitelist by filtering down from CMAKE_PROPERTY_LIST in case cmake is
# a different version, and one of our hardcoded whitelisted properties
# doesn't exist!
unset(CMAKE_WHITELISTED_PROPERTY_LIST)

foreach(prop ${CMAKE_PROPERTY_LIST})
    if(prop MATCHES "^(INTERFACE|[_a-z]|IMPORTED_LIBNAME_|MAP_IMPORTED_CONFIG_)|^(COMPATIBLE_INTERFACE_(BOOL|NUMBER_MAX|NUMBER_MIN|STRING)|EXPORT_NAME|IMPORTED(_GLOBAL|_CONFIGURATIONS|_LIBNAME)?|NAME|TYPE|NO_SYSTEM_FROM_IMPORTED)$")
        list(APPEND CMAKE_WHITELISTED_PROPERTY_LIST ${prop})
    endif()
endforeach(prop)

function(print_target_properties tgt)
    if(NOT TARGET ${tgt})
        message("There is no target named '${tgt}'")
        return()
    endif()

    get_target_property(target_type ${tgt} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        set(PROP_LIST ${CMAKE_WHITELISTED_PROPERTY_LIST})
    else()
        set(PROP_LIST ${CMAKE_PROPERTY_LIST})
    endif()

    foreach (prop ${PROP_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" prop ${prop})
        # message ("Checking ${prop}")
        get_property(propval TARGET ${tgt} PROPERTY ${prop} SET)
        if (propval)
            get_target_property(propval ${tgt} ${prop})
            message ("${tgt} ${prop} = ${propval}")
        endif()
    endforeach(prop)
endfunction(print_target_properties)

print_target_properties(minizip)
print_target_properties(MINIZIP::minizip)
print_target_properties(minizip-ng)
print_target_properties(minizipng)


include(GNUInstallDirs)
# generate and install export file
set(TARGET_LIBS #No alias
        solver-lib
        args_helper
        date
        benchmarking
        result_writer
        sys
        infoCollection
        checks
        misc
        yuni-static-uuid
        yuni-static-core
        ${CMAKE_THREADS_LIBS_INIT}
        antares-solver-main-economy
        antares-solver-main-adequacy
        antares-solver-hydro
        antares-solver-variable
        antares-solver-simulation
        antares-solver-ts-generator
        model_antares
        antares-core
        #deps of deps
        exception
        logs
        infeasible_problem_analysis
        study
        array
        utils
        concurrency
        mersenne
        application
        AntaresMemory
        jit
        io
        series
        object
        path
        correlation
        StudyUtils
        stdcxx
        inifile
        #
        signal-handling
        #minizip target does not exist
)

install(TARGETS ${TARGET_LIBS}
        EXPORT AntaresTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/antares # include/SomeLibrary
        INCLUDES DESTINATION include
)

install(EXPORT AntaresTargets
        FILE "AntaresTargets.cmake"
        NAMESPACE Antares::
        DESTINATION lib/cmake/Antares
)

include(CMakePackageConfigHelpers)

# generate the version file for the config file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/AntaresConfigVersion.cmake"
        VERSION "${version}"
        COMPATIBILITY AnyNewerVersion
)
# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/AntaresConfig.cmake"
        INSTALL_DESTINATION cmake
)
# install config files
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/AntaresConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/AntaresConfigVersion.cmake"
        DESTINATION cmake
)
# generate the export targets for the build tree
export(EXPORT AntaresTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/AntaresTargets.cmake"
        NAMESPACE Antares::
)