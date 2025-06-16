// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <string>

#include <antares/logs/logs.h>
#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"

#include "../../libs/antares/scenarioBuilderExpression/ScenarioGroupParser.h"

namespace Antares::Solver::LoadFiles
{
namespace
{
const std::filesystem::path SCENARIO_BUILDER_PATH = "input/data-series/modeler-scenariobuilder.dat";
}

Optimization::ScenarioGroupRepository loadScenarioGroupRepository(const std::string& studyPath)
{
    try
    {
        // Read file line by line and add scenario for each line
        Optimization::ScenarioGroupRepository scenarioGroupRepository;
        auto file_path = studyPath / SCENARIO_BUILDER_PATH;
        if (!std::filesystem::exists(file_path))
        {
            logs.info() << "No scenario builder found, skipping scenario groups loading.";
            return scenarioGroupRepository;
        }
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            throw std::runtime_error(fmt::format("Could not open {}", file_path.string()));
        }
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
                if (!alreadyCreatedScenarios.contains(parsedLine.groupName))
                {
                    alreadyCreatedScenarios[parsedLine.groupName] = std::make_unique<
                      Optimisation::LinearProblemDataImpl::Scenario>(parsedLine.groupName);
                }
                alreadyCreatedScenarios[parsedLine.groupName]->setChronicle(parsedLine.year,
                                                                            parsedLine.chronicle);
            }
            catch (const std::exception& e)
            {
                logs.error() << "Error parsing line: " << line << " - "
                             << e.what(); // TODO: stack errors and log all ?
                continue;                 // Skip lines that cannot be parsed
            }
        }
        for (auto& [groupId, scenario]: alreadyCreatedScenarios)
        {
            scenarioGroupRepository.addScenario(groupId, std::move(scenario));
        }
        return scenarioGroupRepository;
    }
    catch (const std::exception& e)
    {
        // data-series are not mandatory
        logs.info() << "Data-series could not be loaded: " << e.what();
        return {};
    }
}
} // namespace Antares::Solver::LoadFiles
