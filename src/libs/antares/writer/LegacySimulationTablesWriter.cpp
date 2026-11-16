// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/writer/LegacySimulationTablesWriter.h"

#include <string_view>

#include <antares/exception/RuntimeError.hpp>
#include <antares/io/outputs/SimulationTablePath.h>
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
    return IO::Outputs::simulationTableOutputDirectory(parentFolder) / filename;
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

StreamingSimulationTableWriter& LegacySimulationTablesWriter::streamFor(Stage stage)
{
    auto it = streams_.find(stage);
    if (it == streams_.end())
    {
        auto filepath = makeSimuTableFilePath(folder_, year_, stageName(stage));
        it = streams_.emplace(stage, StreamingSimulationTableWriter(filepath, tableFormat_)).first;
    }
    return it->second;
}

void LegacySimulationTablesWriter::flush(const OptimisationsSimulationTable& tables)
{
    for (const auto& [stage, table]: tables.stages())
    {
        // A stage with no rows produced nothing (yet): no file is created for it
        if (table.rowCount() == 0)
        {
            continue;
        }
        streamFor(stage).append(table);
        table.clear();
    }
}

void LegacySimulationTablesWriter::finish(const OptimisationsSimulationTable& tables)
{
    flush(tables);
    for (const auto& [stage, table]: tables.stages())
    {
        if (!streams_.contains(stage))
        {
            // Writing a header-only file would suggest the stage ran and found nothing.
            logs.info() << fmt::format(
              "No content for stage '{}', skipping writing corresponding simulation table",
              stageName(stage));
        }
    }
    for (auto& [stage, stream]: streams_)
    {
        stream.close();
    }
    streams_.clear();
}

void LegacySimulationTablesWriter::write(const OptimisationsSimulationTable& tables)
{
    for (const auto& [stage, table]: tables.stages())
    {
        if (table.rowCount() == 0)
        {
            continue;
        }
        streamFor(stage).append(table);
    }
    for (const auto& [stage, table]: tables.stages())
    {
        if (!streams_.contains(stage))
        {
            logs.info() << fmt::format(
              "No content for stage '{}', skipping writing corresponding simulation table",
              stageName(stage));
        }
    }
    for (auto& [stage, stream]: streams_)
    {
        stream.close();
    }
    streams_.clear();
}

} // namespace Antares::Writer
