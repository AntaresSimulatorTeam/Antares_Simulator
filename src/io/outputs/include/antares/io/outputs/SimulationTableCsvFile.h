// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>
#include <fstream>

#include "SimulationTableCsv.h"

namespace Antares::IO::Outputs
{
class SimulationTableCsvFile final: public SimulationTableCsv
{
public:
    SimulationTableCsvFile(const std::filesystem::path& outputFolder,
                           const std::string& simulationId);

    void write() override;

private:
    std::filesystem::path output_file_;
};
} // namespace Antares::IO::Outputs
