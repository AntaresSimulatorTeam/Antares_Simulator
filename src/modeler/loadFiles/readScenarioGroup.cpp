// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <string>

#include <boost/algorithm/string.hpp>

#include <antares/logs/logs.h>
#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/scenarioGroupParser/ScenarioGroupParser.h"

namespace Antares::Solver::LoadFiles
{
namespace
{
const std::filesystem::path SCENARIO_BUILDER_PATH = "input/data-series/modeler-scenariobuilder.dat";
}

Optimisation::ScenarioGroupRepository parseScenarioGroupRepository(std::ifstream file)
{
    Optimisation::ScenarioGroupRepository scenarioGroupRepository;
    ScenarioGroupParser parser;
    std::map<std::string, std::unique_ptr<Optimisation::LinearProblemDataImpl::Scenario>>
      alreadyCreatedScenarios;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue; // Skip empty lines and comments
        }
        try
        {
            auto parsedLine = parser.parseLine(line);

            std::string groupId = parsedLine.groupName;
            boost::to_upper(groupId); // all groups are uppercase

            alreadyCreatedScenarios.emplace(
              groupId,
              std::make_unique<Optimisation::LinearProblemDataImpl::Scenario>(groupId));
            alreadyCreatedScenarios[groupId]->setTimeSerieNumber(parsedLine.year,
                                                                 parsedLine.timeSeriesNumber);
        }
        catch (const std::exception& e)
        {
            logs.error() << "Error parsing line: " << line << " - " << e.what();
        }
    }
    for (auto& [groupId, scenario]: alreadyCreatedScenarios)
    {
        scenarioGroupRepository.addScenario(groupId, std::move(scenario));
    }
    return scenarioGroupRepository;
}

Optimisation::ScenarioGroupRepository loadScenarioGroupRepository(
  const std::filesystem::path& studyPath)
{
    try
    {
        // Read file line by line and add scenario for each line

        auto file_path = studyPath / SCENARIO_BUILDER_PATH;
        if (!std::filesystem::exists(file_path))
        {
            logs.info() << "No scenario builder found, skipping scenario groups loading.";
            return {};
        }
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            throw Error::RuntimeError(fmt::format("Could not open {}", file_path.string()));
        }
        return parseScenarioGroupRepository(std::move(file));
    }
    catch (const std::exception& e)
    {
        // scenario-builder is not mandatory
        // this code is probably unreachable
        logs.info() << "Scenario-builder could not be loaded: " << e.what();
        return {};
    }
}
} // namespace Antares::Solver::LoadFiles
