set(SOURCES
        FileWriter.cpp
        include/antares/solver/modeler/fileWriter/FileWriter.h)

# Create the library
add_library(write-modeler-files ${SOURCES})
add_library(Antares::write-modeler-files ALIAS write-modeler-files)

# Specify include directories
target_include_directories(write-modeler-files
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

find_package(fmt REQUIRED)
# Link dependencies (if any)
target_link_libraries(write-modeler-files
        PUBLIC
        Antares::antares-study-system-model
        Antares::linear-problem-api
        Antares::linear-problem-data-impl
        parquet_writer
        PRIVATE
        Antares::io
        Antares::modeler-lib
)

install(DIRECTORY include/antares
        DESTINATION "include"
)