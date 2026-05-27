// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/io/outputs/SimulationTable.h"

// gp : No namespace here. We should add one (Antares::IO::Outputs).
// gp : Besides, this class could be a struct now

class OptimisationsSimulationTable
{
public:
    Antares::IO::Outputs::SimulationTable* firstOptimSimulationTable();
    Antares::IO::Outputs::SimulationTable* secondOptimSimulationTable();
    void clear();

private:
    Antares::IO::Outputs::SimulationTable firstOptimSimulationTable_;
    Antares::IO::Outputs::SimulationTable secondOptimSimulationTable_;
};
