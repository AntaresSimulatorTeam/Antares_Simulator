set(SOURCES
        include/antares/modeler/data/data.h
)

# Create the library
add_library(modeler-data ${SOURCES})
add_library(Antares::modeler-data ALIAS modeler-data)

# Specify include directories
target_include_directories(modeler-data
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(modeler-data
        Antares::optim-model-filler
        Antares::linear-problem-api
        Antares::antares-study-system-model
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
