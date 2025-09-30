/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/io/outputs/SimulationTableCsvFile.h"

#include <antares/logs/logs.h>
#include <antares/writer/table_writer_factory.h>
#include <antares/writer/table_format.h>

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
