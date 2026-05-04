// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/io/outputs/SimulationTable.h"

class OptimisationsSimulationTable
{
public:
    void clear();

    std::pair<std::string, std::string> moveBuffers();

    void write();
    Antares::IO::Outputs::SimulationTable* firstOptimSimulationTable();
    Antares::IO::Outputs::SimulationTable* secondOptimSimulationTable();
    [[nodiscard]] std::string headerCsvFormat() const;

private:
    Antares::IO::Outputs::SimulationTable firstOptimSimulationTable_;
    Antares::IO::Outputs::SimulationTable secondOptimSimulationTable_;
    std::string firstOptimBuffer_;
    std::string secondOptimBuffer_;
};
