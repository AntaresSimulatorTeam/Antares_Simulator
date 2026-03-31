// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/outputs/SimulationTableCsvFile.h"

#include <antares/logs/logs.h>
#include <antares/writer/table_writer_factory.h>

namespace Antares::IO::Outputs
{
SimulationTableCsvFile::SimulationTableCsvFile(const std::filesystem::path& outputFolder,
                                               const std::string& simulationId,
                                               bool parquetFormatRequired):
    parquetFormatRequired_(parquetFormatRequired)
{
    if (!std::filesystem::exists(outputFolder))
    {
        throw std::runtime_error("could not find output Folder: " + outputFolder.string());
    }
    const auto simulation_id = std::string(simulationId.empty() ? "" : "--" + simulationId);
    output_file_ = outputFolder / ("simulation_table" + simulation_id);
    std::string ext = parquetFormatRequired_ ? "parquet" : "csv";
    output_file_ += "." + ext;

    Antares::logs.info() << "Simulation table is written in: " << output_file_.string();
}

void SimulationTableCsvFile::write(const SimulationTableCsv& simuTable)
{
    // Build table vectors
    std::vector<std::string> header = simuTable.rawHeader();
    std::vector<std::vector<std::string>> rows = simuTable.storageIntoRows();

    auto writer = Antares::Writer::makeTableWriter(parquetFormatRequired_);
    writer->writeTable(output_file_, header, rows);
}
} // namespace Antares::IO::Outputs
