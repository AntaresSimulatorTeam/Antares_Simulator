// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/outputs/SimulationTableCsvFile.h"

#include <antares/logs/logs.h>
#include <antares/writer/table_format.h>
#include <antares/writer/table_writer_factory.h>

namespace Antares::IO::Outputs
{
SimulationTableCsvFile::SimulationTableCsvFile(const std::filesystem::path& outputFolder,
                                               const std::string& simulationId):
    SimulationTableCsv()
{
    if (!std::filesystem::exists(outputFolder))
    {
        throw std::runtime_error("could not find output Folder: " + outputFolder.string());
    }
    const auto simulationTableSuffix = std::string(simulationId.empty() ? "" : "--" + simulationId)
                                       + ".csv";
    output_file_ = outputFolder / ("simulation_table" + simulationTableSuffix);
    Antares::logs.info() << "Simulation table is written in: " << output_file_.string();
}

void SimulationTableCsvFile::write()
{
    // Build table vectors
    std::vector<std::string> header;
    std::vector<std::vector<std::string>> rows;
    // gp : Here we convert to row format, we don't "export". To be renamed.
    // gp : Plus : this function should return the row format for more clarity.
    exportTable(header, rows);

    // Choose writer based on file extension
    Antares::Writer::TableFormat fmt = Antares::Writer::TableFormat::CSV;
    if (output_file_.extension() == ".parquet")
    {
        fmt = Antares::Writer::TableFormat::Parquet;
    }
    auto writer = Antares::Writer::makeTableWriter(fmt);
    writer->writeTable(output_file_, header, rows);
}
} // namespace Antares::IO::Outputs
