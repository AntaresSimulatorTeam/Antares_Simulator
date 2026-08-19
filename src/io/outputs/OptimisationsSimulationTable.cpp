// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/io/outputs/OptimisationsSimulationTable.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace Antares::IO::Outputs
{
const std::vector<std::string>& OptimisationsSimulationTable::allStages()
{
    static const std::vector<std::string> stages = {firstOptimStage,
                                                    secondOptimStage,
                                                    remixHydroStage,
                                                    adequacyPatchStage};
    return stages;
}

std::set<std::string> OptimisationsSimulationTable::parseStageSelection(const std::string& input,
                                                                        const std::string& source)
{
    std::set<std::string> selection;

    std::istringstream stream(input);
    std::string name;
    while (std::getline(stream, name, ','))
    {
        // Tolerate the spaces a user naturally writes after a comma.
        const auto first = name.find_first_not_of(" \t");
        if (first == std::string::npos)
        {
            continue;
        }
        name = name.substr(first, name.find_last_not_of(" \t") - first + 1);

        if (name == "all")
        {
            return {};
        }

        const auto& known = allStages();
        if (std::find(known.begin(), known.end(), name) == known.end())
        {
            std::ostringstream message;
            message << "Invalid value for " << source << ": '" << name << "' (expected all";
            for (const auto& stage: known)
            {
                message << ", " << stage;
            }
            message << ")";
            throw std::runtime_error(message.str());
        }
        selection.insert(name);
    }

    return selection;
}

void OptimisationsSimulationTable::selectStages(std::set<std::string> stages)
{
    selectedStages_ = std::move(stages);
}

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
    if (!isStageSelected(stage))
    {
        return nullptr;
    }
    return &stages_.try_emplace(stage).first->second;
}

bool OptimisationsSimulationTable::isStageSelected(const std::string& stage) const
{
    return selectedStages_.empty() || selectedStages_.contains(stage);
}

bool OptimisationsSimulationTable::anyPostProcessStageSelected() const
{
    return isStageSelected(remixHydroStage) || isStageSelected(adequacyPatchStage);
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
