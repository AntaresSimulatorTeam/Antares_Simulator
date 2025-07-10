set(SOURCES
        include/antares/modeler/data/data.h
)

# Create the library
add_library(modeler-data INTERFACE ${SOURCES})
add_library(Antares::modeler-data ALIAS modeler-data)

# Specify include directories
target_include_directories(modeler-data INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(modeler-data INTERFACE
        Antares::optim-model-filler
        Antares::linear-problem-api
        Antares::antares-study-system-model
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
