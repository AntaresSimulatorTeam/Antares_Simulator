/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/io/outputs/SimulationTableGenerator.h"

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/VariableDictionary.h>
#include <antares/study/system-model/component.h>
#include "antares/optimisation/linear-problem-api/mipSolution.h"

void FillSimulationTable(
  ISimulationTable& simulationTable,
  const Antares::Optimisation::LinearProblemApi::IMipSolution& solution,
  const std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component>& components,
  const Antares::Optimization::VariableDictionary& variableDictionary,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext)
{
    for (const auto& [componentId, component]: components)
    {
        for (const auto& [var_name, modelVar]: component.getModel()->Variables())
        {
            if (modelVar.IsScenarioDependent() && modelVar.isTimeDependent())
            {
                // TODO
                //  for (auto scenario: fillContext.getSelectedScenarios())
                unsigned int scenario = 0;
                {
                    for (auto timeStep(fillContext.getFirstTimeStep());
                         timeStep <= fillContext.getLastTimeStep();
                         ++timeStep)
                    {
                        auto* var = variableDictionary(componentId, var_name, scenario, timeStep);
                        simulationTable.addEntry({.block = 1,
                                                  .component = componentId,
                                                  .output = var_name,
                                                  .absolute_time_index = timeStep,
                                                  .block_time_index = timeStep,
                                                  .scenario_index = scenario,
                                                  .value = solution.getOptimalValue(var)});
                    }
                }
            }
            else if (modelVar.IsScenarioDependent())
            {
                for (auto scenario: fillContext.getSelectedScenarios())
                {
                    auto* var = variableDictionary(componentId, var_name, scenario, 0);
                    simulationTable.addEntry({.block = 1,
                                              .component = componentId,
                                              .output = var_name,
                                              .absolute_time_index = std::nullopt,
                                              .block_time_index = std::nullopt,
                                              .scenario_index = scenario,
                                              .value = solution.getOptimalValue(var)});
                }
            }
            else if (modelVar.isTimeDependent())
            {
                for (auto timeStep: fillContext.getSelectedScenarios())
                {
                    auto* var = variableDictionary(componentId, var_name, 0, timeStep);
                    simulationTable.addEntry({.block = 1,
                                              .component = componentId,
                                              .output = var_name,
                                              .absolute_time_index = timeStep,
                                              .block_time_index = timeStep,
                                              .scenario_index = std::nullopt,
                                              .value = solution.getOptimalValue(var)});
                }
            }

            else
            {
                auto* var = variableDictionary(componentId, var_name);
                simulationTable.addEntry({.block = 1,
                                          .component = componentId,
                                          .output = var_name,
                                          .absolute_time_index = std::nullopt,
                                          .block_time_index = std::nullopt,
                                          .scenario_index = std::nullopt,
                                          .value = solution.getOptimalValue(var)});
            }
        }
    }
}
