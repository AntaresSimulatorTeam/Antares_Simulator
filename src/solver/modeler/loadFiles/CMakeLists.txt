set(SOURCES
        readSystem.cpp
        readLibraries.cpp
        readParameters.cpp
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
        PRIVATE
        Antares::io
        Antares::systemParser
        Antares::modelParser
        Antares::modelConverter
        Antares::modelerParameters
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
