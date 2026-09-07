// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

#include "antares/io/outputs/SimulationTable.h"

namespace Antares::Optimization
{
class InactiveComponentsAnalyzer;
}

namespace Antares::IO::Outputs
{
class OptimisationsSimulationTable
{
public:
    SimulationTable* firstOptimSimulationTable();
    SimulationTable* secondOptimSimulationTable();
    void clear();

    std::shared_ptr<const Optimization::InactiveComponentsAnalyzer> inactiveComponents;

private:
    SimulationTable firstOptimSimulationTable_;
    SimulationTable secondOptimSimulationTable_;
};
} // namespace Antares::IO::Outputs
