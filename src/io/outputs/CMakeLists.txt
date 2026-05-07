# ===================================
# ==== target : simulation-table ====
# ===================================
set(SRC_SIMU_TABLE
        SimulationTable.cpp
        include/antares/io/outputs/SimulationTableEntry.h
        include/antares/io/outputs/SimulationTable.h
        include/antares/io/outputs/columns.h
        include/antares/io/outputs/storage.h)

add_library(simulation-table STATIC ${SRC_SIMU_TABLE})
add_library(Antares::simulation-table ALIAS simulation-table)

set_target_properties(simulation-table PROPERTIES
        POSITION_INDEPENDENT_CODE ON)

target_include_directories(simulation-table
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(simulation-table
        PUBLIC
        Antares::linear-problem-api)


# =======================================
# ==== target : simu-table-generator ====
# =======================================

set(SRC_SIMU_TABLE_GENERATOR
        SimulationTableGenerator.cpp
        MPSGenerator.cpp
        ExportableName.cpp
        include/antares/io/outputs/SimulationTableGenerator.h
        include/antares/io/outputs/MPSGenerator.h
        include/antares/io/outputs/ExportableName.h)

# Create the library
add_library(simu-table-generator STATIC ${SRC_SIMU_TABLE_GENERATOR})
add_library(Antares::simu-table-generator ALIAS simu-table-generator)

# Specify include directories
target_include_directories(simu-table-generator
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies
target_link_libraries(simu-table-generator
        PUBLIC
        Antares::linear-problem-data-impl
        Antares::optim-model-filler
        Antares::antares-study-system-model
        Antares::modeler-lib
        Antares::study-utils
        Antares::simulation-table
        PRIVATE
        Antares::exception
        Antares::result_writer
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
