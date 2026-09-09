// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/io/outputs/OptimisationsSimulationTable.h"

namespace Antares::IO::Outputs
{
SimulationTable* OptimisationsSimulationTable::firstOptimSimulationTable()
{
    return tableForStage(firstOptimStage);
}

SimulationTable* OptimisationsSimulationTable::secondOptimSimulationTable()
{
    return tableForStage(secondOptimStage);
}

SimulationTable* OptimisationsSimulationTable::tableForStage(const std::string& stage)
{
    return &stages_.try_emplace(stage).first->second;
}

const std::map<std::string, SimulationTable>& OptimisationsSimulationTable::stages() const
{
    return stages_;
}

void OptimisationsSimulationTable::clear()
{
    for (auto& [stage, table]: stages_)
    {
        table.clear();
    }
}
} // namespace Antares::IO::Outputs
