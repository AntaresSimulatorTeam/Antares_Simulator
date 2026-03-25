##########################
# This file contains the instructions to generate the Antares package and install it
# Installing a target requires to know its dependency and so on thus the long list below
#

include(GNUInstallDirs)
# generate and install export file
set(TARGET_LIBS #No alias
        # each "block" of dependency describe the dependency for a target
        # not dependency is present since once a dependency is in the export set
        # it is available for everything

        solver_api # What we want to export
		single_problem_api
		antares-problem-generator
        study
        scenario-builder-utils
        study-loader
        file-tree-study-loader
        antares-solver-simulation
        # study
        yuni-static-core
        array
        date
        inifile
        study-utils
		vector-utils
        utils
        correlation
        path
        mersenne
        result_writer
        series
        antares-core
        antares-solver-hydro
        exception
        benchmarking
        antares-solver-variable
        lps

        # study-loader : nothing

        application

        # run-mode
        infoCollection

        # antares-solver-simulation
        concurrency
        misc
        model_antares
        antares-solver-ts-generator
        shave-peaks-by-remix-storage-gen
        storage-for-remix-list-sort

        # lps : nothing

        # array
        io
        jit
        AntaresMemory

        # date
        logs

        # correlation
        locator

        # antares-core
        antares-config-lib

        # application
        solver-lib
        sys
        signal-handling
        antares-solver-variable-info
        optimization-options
        resources

        # model_antares
        infeasible_problem_analysis
        linear-problem-api
        linear-problem-data-impl
        linear-problem-mpsolver-impl
        modeler-lib

        # solver-lib
        args_helper
        checks
        locale
        yuni-static-uuid

        # executable
        antares-solver

        # modeler libs
        antlr-interface
        antares-study-system-model
        load-modeler-files
		write-modeler-files
        modeler-optimisation-container
        expressions
        optim-model-filler
		forbidden-nodes
        yml-system
        yml-model
		yml-utils
        model-converter
        modeler-parameters
        data-series-csv-importer
        expressions-iterators
        additional-constraint-rhs-antlr-interface
        modeler-simulation-table
        scenario-builder-antlr
        scenario-group-parser
        yml-optim-config
        enums
        modeler-checks
		structured-linear-problem
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
