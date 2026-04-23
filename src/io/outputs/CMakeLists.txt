set(SOURCES
        SimulationTableCsv.cpp
        SimulationTableCsvFile.cpp
        SimulationTableGenerator.cpp
        MPSGenerator.cpp
        ExportableName.cpp
        include/antares/io/outputs/SimulationTableEntry.h
        include/antares/io/outputs/ISimulationTable.h
        include/antares/io/outputs/SimulationTableCsv.h
        include/antares/io/outputs/SimulationTableCsvFile.h
        include/antares/io/outputs/SimulationTableGenerator.h
        include/antares/io/outputs/columns.h
        include/antares/io/outputs/storage.h
        include/antares/io/outputs/MPSGenerator.h
        include/antares/io/outputs/ExportableName.h
)

# Create the library
add_library(modeler-simulation-table STATIC ${SOURCES})
add_library(Antares::modeler-simulation-table ALIAS modeler-simulation-table)

# Specify include directories
target_include_directories(modeler-simulation-table
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies
target_link_libraries(modeler-simulation-table
        PUBLIC
        Antares::linear-problem-data-impl
        Antares::optim-model-filler
        Antares::antares-study-system-model
        Antares::modeler-lib
        Antares::study-utils
        PRIVATE
        Antares::exception
        Antares::result_writer
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
