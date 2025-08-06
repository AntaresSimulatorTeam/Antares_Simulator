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

#include <antares/solver/optim-model-filler/VariableDictionary.h>
#include "antares/optimisation/linear-problem-api/linearProblem.h"
#include "antares/optimisation/linear-problem-api/mipConstraint.h"
#include "antares/optimisation/linear-problem-api/mipSolution.h"

namespace operations_research
{
class MPConstraint;
class MPVariable;
} // namespace operations_research

using namespace Antares::Optimisation::LinearProblemApi;

TimeBlock convertTimeStepToBlockTimeIndex(unsigned int timeStep, const TimeConversionMode& mode)
{
    switch (mode)
    {
    case TimeConversionMode::WeeklyBlocks:
        return {.block = timeStep / 168,
                .blockTimeIndex = timeStep % 168,
                .absoluteTimeIndex = timeStep};
    case TimeConversionMode::DailyBlocks:
        return {.block = timeStep / 24,
                .blockTimeIndex = timeStep % 24,
                .absoluteTimeIndex = timeStep};
    case TimeConversionMode::SingleBlock:
    default:
        return {.block = 1, .blockTimeIndex = timeStep, .absoluteTimeIndex = timeStep};
    }
}

struct ModelerSolverTraits
{
    static double getValue(const Antares::Optimisation::LinearProblemApi::IMipVariable* var)
    {
        return var->solutionValue();
    }

    static MipBasisStatus getStatus(
      const Antares::Optimisation::LinearProblemApi::IMipVariable* var)
    {
        return var->getMipBasisStatus();
    }

    static MipBasisStatus getStatus(
      const Antares::Optimisation::LinearProblemApi::IMipConstraint* cst)
    {
        return cst->getMipBasisStatus();
    }

    static std::optional<double> getValue(
      const Antares::Optimisation::LinearProblemApi::IMipConstraint*)
    {
        return std::nullopt;
    }
};

//
// void addVariableEntries(ISimulationTable& simulationTable,
//                         const Antares::Optimisation::LinearProblemApi::IMipSolution& solution,
//                         const Antares::Optimization::VariableDictionary& variableDictionary,
//                         const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
//                         const Antares::ModelerStudy::SystemModel::Component& component)
// {
//     const auto componentId = component.Id();
//
//     for (const auto& [var_name, modelVar]: component.getModel()->Variables())
//     {
//         if (modelVar.IsScenarioDependent() && modelVar.isTimeDependent())
//         {
//             // TODO
//             //  for (auto scenario: fillContext.getSelectedScenarios())
//             unsigned int scenario = 0;
//             {
//                 for (auto timeStep(fillContext.getFirstTimeStep());
//                      timeStep <= fillContext.getLastTimeStep();
//                      ++timeStep)
//                 {
//                     auto* var = variableDictionary(componentId, var_name, scenario, timeStep);
//                     simulationTable.addEntry({.block = 1,
//                                               .component = componentId,
//                                               .output = var_name,
//                                               .absolute_time_index = timeStep,
//                                               .block_time_index = timeStep,
//                                               .scenario_index = scenario,
//                                               .value = solution.getOptimalValue(var),
//                                               .status = var->getMipBasisStatus()});
//                 }
//             }
//         }
//         else if (modelVar.IsScenarioDependent())
//         {
//             for (auto scenario: fillContext.getSelectedScenarios())
//             {
//                 auto* var = variableDictionary(componentId, var_name, scenario, 0);
//                 simulationTable.addEntry({.block = 1,
//                                           .component = componentId,
//                                           .output = var_name,
//                                           .absolute_time_index = std::nullopt,
//                                           .block_time_index = std::nullopt,
//                                           .scenario_index = scenario,
//                                           .value = solution.getOptimalValue(var),
//                                           .status = var->getMipBasisStatus()});
//             }
//         }
//         else if (modelVar.isTimeDependent())
//         {
//             for (auto timeStep(fillContext.getFirstTimeStep());
//                  timeStep <= fillContext.getLastTimeStep();
//                  ++timeStep)
//             {
//                 auto* var = variableDictionary(componentId, var_name, 0, timeStep);
//                 simulationTable.addEntry({.block = 1,
//                                           .component = componentId,
//                                           .output = var_name,
//                                           .absolute_time_index = timeStep,
//                                           .block_time_index = timeStep,
//                                           .scenario_index = std::nullopt,
//                                           .value = solution.getOptimalValue(var),
//                                           .status = var->getMipBasisStatus()});
//             }
//         }
//
//         else
//         {
//             auto* var = variableDictionary(componentId, var_name);
//             simulationTable.addEntry({.block = 1,
//                                       .component = componentId,
//                                       .output = var_name,
//                                       .absolute_time_index = std::nullopt,
//                                       .block_time_index = std::nullopt,
//                                       .scenario_index = std::nullopt,
//                                       .value = solution.getOptimalValue(var),
//                                       .status = var->getMipBasisStatus()});
//         }
//     }
// }
//
// void addConstraintEntries(
//   ISimulationTable& simulationTable,
//   const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
//   const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
//   const Antares::ModelerStudy::SystemModel::Component& component)
// {
//     const auto& componentId = component.Id();
//
//     for (const auto& [name, modelConstraint]: component.getModel()->Constraints())
//     {
//         switch (Antares::Expressions::Visitors::TimeIndexVisitor timeIndexVisitor(component);
//                 timeIndexVisitor.dispatch(modelConstraint.expression().RootNode()))
//         {
//         case Antares::Expressions::Visitors::TimeIndex::VARYING_IN_TIME_AND_SCENARIO:
//         {
//             // TODO
//             //  for (auto scenario: fillContext.getSelectedScenarios())
//             unsigned int scenario = 0;
//             {
//                 for (auto timeStep(fillContext.getFirstTimeStep());
//                      timeStep <= fillContext.getLastTimeStep();
//                      ++timeStep)
//                 {
//                     const auto* constr = linearProblem.lookupConstraint(
//                       component.Id() + "." + name + '_' + std::to_string(timeStep));
//                     simulationTable.addEntry({.block = 1,
//                                               .component = componentId,
//                                               .output = name,
//                                               .absolute_time_index = timeStep,
//                                               .block_time_index = timeStep,
//                                               .scenario_index = scenario,
//                                               .value = std::nullopt,
//                                               .status = constr->getMipBasisStatus()});
//                 }
//             }
//         }
//         break;
//         case Antares::Expressions::Visitors::TimeIndex::VARYING_IN_SCENARIO_ONLY:
//         {
//             for (auto scenario: fillContext.getSelectedScenarios())
//             {
//                 // TODO
//                 const auto* constr = linearProblem.lookupConstraint(
//                   component.Id() + "." + name + '_' + std::to_string(scenario));
//                 simulationTable.addEntry({.block = 1,
//                                           .component = componentId,
//                                           .output = name,
//                                           .absolute_time_index = std::nullopt,
//                                           .block_time_index = std::nullopt,
//                                           .scenario_index = scenario,
//                                           .value = std::nullopt,
//                                           .status = constr->getMipBasisStatus()});
//             }
//         }
//         break;
//
//         case Antares::Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY:
//         {
//             for (auto timeStep(fillContext.getFirstTimeStep());
//                  timeStep <= fillContext.getLastTimeStep();
//                  ++timeStep)
//             {
//                 // TODO
//                 const auto* constr = linearProblem.lookupConstraint(
//                   component.Id() + "." + name + '_' + std::to_string(timeStep));
//                 simulationTable.addEntry({.block = 1,
//                                           .component = componentId,
//                                           .output = name,
//                                           .absolute_time_index = timeStep,
//                                           .block_time_index = timeStep,
//                                           .scenario_index = std::nullopt,
//                                           .value = std::nullopt,
//                                           .status = constr->getMipBasisStatus()});
//             }
//         }
//         break;
//
//         case Antares::Expressions::Visitors::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO:
//         default: /*TODO*/
//         {
//             // TODO
//             const auto* constr = linearProblem.lookupConstraint(component.Id() + "." + name);
//
//             simulationTable.addEntry({.block = 1,
//                                       .component = componentId,
//                                       .output = name,
//                                       .absolute_time_index = std::nullopt,
//                                       .block_time_index = std::nullopt,
//                                       .scenario_index = std::nullopt,
//                                       .value = std::nullopt,
//                                       .status = constr->getMipBasisStatus()});
//         }
//         break;
//         }
//     }
// }

// void FillSimulationTable(
//   ISimulationTable& simulationTable,
//   const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
//   const Antares::Optimisation::LinearProblemApi::IMipSolution& solution,
//   const std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component>&
//   components, const Antares::Optimization::VariableDictionary& variableDictionary, const
//   Antares::Optimisation::LinearProblemApi::FillContext& fillContext)
// {
//     for (const auto& component: components | std::views::values)
//     {
//         addVariableEntries(simulationTable, solution, variableDictionary, fillContext,
//         component); addConstraintEntries(simulationTable, linearProblem, fillContext, component);
//     }
// }
std::string BuildModelerConstraintName(const std::string& cid,
                                       const std::string& cname,
                                       const std::optional<unsigned>& scen,
                                       const std::optional<unsigned>& ts)
{
    // TODO
    //  if (scen)
    //      key += "_s" + std::to_string(*scen);
    //  if (ts)
    //      key += "_t" + std::to_string(*ts);

    // TODO
    std::string key = cid + "." + cname;
    if (ts)
    {
        key += "_" + std::to_string(*ts); // TODO
    }
    else if (scen)
    {
        key += "_" + std::to_string(*scen); // TODO
    }
    return key;
}
void FillSimulationTable(
  ISimulationTable& simulationTable,
  const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
  const Antares::Optimisation::LinearProblemApi::IMipSolution& solution,
  const std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component>& components,
  const Antares::Optimization::VariableDictionary& variableDictionary,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext)
{
    auto variableLookupModeler = [&](const std::string& cid,
                                     const std::string& vname,
                                     std::optional<unsigned> scen,
                                     std::optional<unsigned> ts)
      -> const Antares::Optimisation::LinearProblemApi::IMipVariable*
    { return variableDictionary(cid, vname, scen.value_or(0), ts.value_or(0)); };

    auto constraintLookupModeler = [&](const std::string& cid,
                                       const std::string& cname,
                                       std::optional<unsigned> scen,
                                       std::optional<unsigned> ts)
      -> const Antares::Optimisation::LinearProblemApi::IMipConstraint*
    { return linearProblem.lookupConstraint(BuildModelerConstraintName(cid, cname, scen, ts)); };

    for (const auto& component: components | std::views::values)
    {
        // TODO
        unsigned scenario = 0;
        addVariableEntries<ModelerSolverTraits>(simulationTable,
                                                fillContext,
                                                component,
                                                variableLookupModeler,
                                                1 /* or your block */,
                                                TimeConversionMode::SingleBlock,
                                                scenario);

        addConstraintEntries<ModelerSolverTraits>(simulationTable,
                                                  fillContext,
                                                  component,
                                                  constraintLookupModeler,
                                                  1 /* or your block */,
                                                  TimeConversionMode::SingleBlock,
                                                  scenario);
    }
}
