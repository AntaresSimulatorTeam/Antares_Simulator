##########################
# This file contains the instructions to generate the Antares package and install it
# Installing a target requires to know its dependency and so on thus the long list below
#

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
        antares-solver-hydro
        antares-solver-variable
        antares-solver-simulation
        antares-solver-ts-generator
        model_antares
        antares-core
        locale
        resources
        locator
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
        path
        correlation
        StudyUtils
        stdcxx
        inifile
        antares-config-lib
        file-tree-study-loader
        study-loader
        lps
        #
        signal-handling
        antares-solver-variable-info
)

install(TARGETS ${TARGET_LIBS}
        EXPORT AntaresTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
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
        COMPATIBILITY SameMajorVersion
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
        DESTINATION lib/cmake/Antares
)