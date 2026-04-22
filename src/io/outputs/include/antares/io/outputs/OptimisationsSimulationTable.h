// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/io/outputs/SimulationTable.h"

class OptimisationsSimulationTable
{
public:
    void clear();

// gp : No namespace here. We should add one (Antares::IO::Outputs).
// gp : Besides, this class could be a struct now

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
