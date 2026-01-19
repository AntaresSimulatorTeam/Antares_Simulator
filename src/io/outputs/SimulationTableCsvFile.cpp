// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/outputs/SimulationTableCsvFile.h"

#include <antares/logs/logs.h>

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
    const auto simulationTableFile = outputFolder / ("simulation_table" + simulationTableSuffix);
    file_.open(simulationTableFile);
    if (!file_.is_open())
    {
        throw std::runtime_error("Cannot open file: " + simulationTableFile.string());
    }
    Antares::logs.info() << "Simulation table is written in: " << simulationTableFile.string();
}

void SimulationTableCsvFile::write()
{
    SimulationTableCsv::write();
    file_ << buffer();
}
