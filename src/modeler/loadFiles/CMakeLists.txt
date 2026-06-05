set(SOURCES
        handleErrors.cpp
        loadAll.cpp
        readDataSeries.cpp
        readLibraries.cpp
        readParameters.cpp
        readDataSeries.cpp
        handleErrors.cpp
        FileLoader.cpp
        readScenarioGroup.cpp
        readSystem.cpp
        readOptimConfig.cpp

        include/antares/solver/modeler/loadFiles/loadFiles.h
        include/antares/solver/modeler/loadFiles/Fileloader.h
        include/antares/solver/modeler/loadFiles/readOptimConfig.h
)

# Create the library
add_library(load-modeler-files ${SOURCES})
add_library(Antares::load-modeler-files ALIAS load-modeler-files)

# Specify include directories
target_include_directories(load-modeler-files
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

include(GNUInstallDirs)
# Bake two paths into the library so loadAll.cpp can find variable-names.yml
# without platform-specific executable-directory detection:
#   - INSTALL path: where `cmake --install` places the file (FHS data dir)
#   - SOURCE path:  absolute path in the source tree, used as a fallback for
#                   development builds where the install prefix may not exist
target_compile_definitions(load-modeler-files PRIVATE
    "ANTARES_VARIABLE_NAMES_INSTALL_PATH=\"${CMAKE_INSTALL_FULL_DATADIR}/antares/variable-names.yml\""
    "ANTARES_VARIABLE_NAMES_SOURCE_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../resources/variable-names.yml\""
)

find_package(fmt REQUIRED)
# Link dependencies (if any)
target_link_libraries(load-modeler-files
        PUBLIC
        Antares::antares-study-system-model
        Antares::linear-problem-api
        Antares::linear-problem-data-impl
        PRIVATE
        Antares::io
        Antares::yml-optim-config
        Antares::yml-system
        Antares::yml-model
        Antares::model-converter
        Antares::modeler-parameters
        Antares::data-series-csv-importer
        Antares::modeler-lib
        Antares::scenario-group-parser
        Antares::study-utils
        Antares::modeler-checks
        fmt::fmt
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
