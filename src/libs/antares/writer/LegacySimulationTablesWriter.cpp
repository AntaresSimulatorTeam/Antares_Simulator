// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/writer/LegacySimulationTablesWriter.h"

#include <antares/exception/RuntimeError.hpp>
#include "antares/writer/simulation_table_writer.h"

namespace fs = std::filesystem;
using namespace Antares::IO::Outputs;
using namespace Antares::Error;

namespace Antares::Writer
{
static void check_is_existing_folder(const fs::path& folder)
{
    if (!fs::is_directory(folder))
    {
        std::string msg = "Output folder " + folder.string() + " does not exist.";
        throw RuntimeError(msg);
    }
}

static fs::path makeSimuTableFilePath(const fs::path& parentFolder,
                                      const unsigned year,
                                      const std::string& stage)
{
    // File name without extension
    std::string filename = "simulation-table-" + std::to_string(year) + "-" + stage;
    return parentFolder / filename;
}

LegacySimulationTablesWriter::LegacySimulationTablesWriter(const fs::path& folder,
                                                           const unsigned year,
                                                           TableFormat tableFormat):
    year_(year),
    tableFormat_(tableFormat),
    folder_(folder)
{
    check_is_existing_folder(folder_);
}

void LegacySimulationTablesWriter::write(OptimisationsSimulationTable& tables)
{
    for (const auto& [stage, table]: tables.stages())
    {
        writeForStage(table, stage);
    }
}

void LegacySimulationTablesWriter::writeForStage(const SimulationTable& table,
                                                 const std::string& stage)
{
    auto filepath = makeSimuTableFilePath(folder_, year_, stage);
    SimulationTableWriter writer(filepath, tableFormat_);
    writer.writeTable(table);
}

} // namespace Antares::Writer
