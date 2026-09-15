// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/io/outputs/OptimisationsSimulationTable.h"

#include <sstream>
#include <string>
#include <vector>

#include <antares/exception/InvalidArgumentError.hpp>

namespace Antares::IO::Outputs
{
namespace
{
// Split on ',', dropping empty fields and trimming the spaces a user naturally
// writes around each name. No business logic here: "all" and unknown names come
// out as-is.
std::vector<std::string> splitStageList(const std::string& input)
{
    std::vector<std::string> names;
    std::istringstream stream(input);
    std::string field;
    while (std::getline(stream, field, ','))
    {
        const auto first = field.find_first_not_of(" \t");
        if (first == std::string::npos)
        {
            continue;
        }
        const auto last = field.find_last_not_of(" \t");
        names.push_back(field.substr(first, last - first + 1));
    }
    return names;
}

[[noreturn]] void rejectUnknownStage(const std::string& name, const std::string& source)
{
    std::ostringstream message;
    message << "Invalid value for " << source << ": '" << name << "' (expected all";
    for (const auto stage: allStages)
    {
        message << ", " << stageName(stage);
    }
    message << ")";
    throw Error::InvalidArgumentError(message.str());
}

// Turn the raw names into stages, applying the "all" keyword. Every name still
// has to be a real stage, so a typo in `all,optim-nb-3` is reported rather than
// swallowed. An empty result means "every stage": that is what "all", and an
// empty list, resolve to.
std::vector<Stage> resolveStages(const std::vector<std::string>& names, const std::string& source)
{
    bool everyStage = false;
    std::vector<Stage> stages;
    for (const auto& name: names)
    {
        if (name == "all")
        {
            everyStage = true;
            continue;
        }

        const auto stage = stageFromName(name);
        if (!stage)
        {
            rejectUnknownStage(name, source);
        }
        stages.push_back(*stage);
    }
    return everyStage ? std::vector<Stage>{} : stages;
}
} // namespace

OptimisationsSimulationTable::OptimisationsSimulationTable(
  std::shared_ptr<const Optimization::InactiveComponentsAnalyzer> inactiveComponents):
    inactiveComponents(std::move(inactiveComponents))
{
}

std::set<Stage> OptimisationsSimulationTable::parseStageSelection(const std::string& input,
                                                                  const std::string& source)
{
    const auto stages = resolveStages(splitStageList(input), source);
    return {stages.begin(), stages.end()};
}

void OptimisationsSimulationTable::selectStages(std::set<Stage> stages)
{
    selectedStages_ = std::move(stages);
}

SimulationTable* OptimisationsSimulationTable::firstOptimSimulationTable()
{
    return tableForStage(Stage::firstOptim);
}

SimulationTable* OptimisationsSimulationTable::secondOptimSimulationTable()
{
    return tableForStage(Stage::secondOptim);
}

SimulationTable* OptimisationsSimulationTable::tableForStage(Stage stage)
{
    if (!isStageSelected(stage))
    {
        return nullptr;
    }
    return &stages_.try_emplace(stage).first->second;
}

bool OptimisationsSimulationTable::isStageSelected(Stage stage) const
{
    return selectedStages_.empty() || selectedStages_.contains(stage);
}

bool OptimisationsSimulationTable::anyPostProcessStageSelected() const
{
    return isStageSelected(Stage::remixHydro) || isStageSelected(Stage::adequacyPatchCsr);
}

const std::map<Stage, SimulationTable>& OptimisationsSimulationTable::stages() const
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
