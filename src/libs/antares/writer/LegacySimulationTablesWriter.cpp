// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/writer/LegacySimulationTablesWriter.h"

#include <string_view>

#include <antares/exception/RuntimeError.hpp>
#include <antares/logs/logs.h>
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
                                      const std::string_view stage)
{
    // File name without extension
    std::string filename = "simulation-table-" + std::to_string(year) + "-" + std::string(stage);
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

void LegacySimulationTablesWriter::write(const OptimisationsSimulationTable& tables) const
{
    for (const auto& [stage, table]: tables.stages())
    {
        // A stage with no rows produced nothing worth reporting -- e.g. a
        // post-process dump that declined to run. Writing it would emit a
        // header-only file suggesting the stage ran and found nothing.
        if (table.rowCount() == 0)
        {
            logs.info() << fmt::format(
              "No content for stage '{}', skipping writing corresponding simulation table",
              stageName(stage));
            continue;
        }
        writeForStage(table, stage);
    }
}

void LegacySimulationTablesWriter::writeForStage(const SimulationTable& table, Stage stage) const
{
    auto filepath = makeSimuTableFilePath(folder_, year_, stageName(stage));
    SimulationTableWriter writer(filepath, tableFormat_);
    writer.writeTable(table);
}

} // namespace Antares::Writer
