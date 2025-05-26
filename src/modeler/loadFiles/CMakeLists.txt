set(SOURCES
        loadAll.cpp
        readSystem.cpp
        readLibraries.cpp
        readParameters.cpp
        readDataSeries.cpp
        handleErrors.cpp

        include/antares/solver/modeler/loadFiles/loadFiles.h
        include/antares/solver/modeler/loadFiles/data.h
)

# Create the library
add_library(load-modeler-files ${SOURCES})
add_library(Antares::load-modeler-files ALIAS load-modeler-files)

# Specify include directories
target_include_directories(load-modeler-files
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(load-modeler-files
        PUBLIC
        Antares::antares-study-system-model
        Antares::linear-problem-api
        Antares::linear-problem-data-impl
        PRIVATE
        Antares::io
        Antares::yml-system
        Antares::yml-model
        Antares::model-converter
        Antares::modeler-parameters
        Antares::data-series-csv-importer
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
