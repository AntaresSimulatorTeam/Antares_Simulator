// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/io/outputs/OptimisationsSimulationTable.h"

namespace Antares::IO::Outputs
{
SimulationTable* OptimisationsSimulationTable::firstOptimSimulationTable()
{
    return &firstOptimSimulationTable_;
}

SimulationTable* OptimisationsSimulationTable::secondOptimSimulationTable()
{
    return &secondOptimSimulationTable_;
}

void OptimisationsSimulationTable::clear()
{
    firstOptimSimulationTable_.clear();
    secondOptimSimulationTable_.clear();
}
} // namespace Antares::IO::Outputs
