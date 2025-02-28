set(SOURCES
        readSystem.cpp
        readLibraries.cpp
        readParameters.cpp
        readDataSeries.cpp
        handleErrors.cpp

        include/antares/solver/modeler/loadFiles/loadFiles.h
)

# Create the library
add_library(loadModelerFiles STATIC ${SOURCES})
add_library(Antares::loadModelerFiles ALIAS loadModelerFiles)

# Specify include directories
target_include_directories(loadModelerFiles
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(loadModelerFiles
        PUBLIC
        Antares::antares-study-system-model
        Antares::linear-problem-api
        PRIVATE
        Antares::io
        Antares::yml-system
        Antares::yml-model
        Antares::model-converter
        Antares::modelerParameters
        Antares::data-series-csv-importer
        linear-problem-data-impl
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
