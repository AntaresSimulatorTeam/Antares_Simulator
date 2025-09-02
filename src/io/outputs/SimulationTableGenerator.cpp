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

#include <optional>

#include <antares/solver/optim-model-filler/VariableDictionary.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/expressions/visitors/TimeIndexVisitor.h"
#include "antares/optimisation/linear-problem-api/IScenario.h"
#include "antares/optimisation/linear-problem-api/linearProblem.h"
#include "antares/optimisation/linear-problem-api/mipConstraint.h"

using namespace Antares::Optimisation::LinearProblemApi;

TimeBlock convertTimeStepToBlockTimeIndex(unsigned int timeStep, const TimeConversionMode& mode)
{
    switch (mode)
    {
    case TimeConversionMode::WeeklyBlocks:
        return {.block = timeStep / 168 + 1,
                .blockTimeIndex = timeStep % 168 + 1,
                .absoluteTimeIndex = timeStep + 1};
    case TimeConversionMode::DailyBlocks:
        return {.block = timeStep / 24 + 1,
                .blockTimeIndex = timeStep % 24 + 1,
                .absoluteTimeIndex = timeStep + 1};
    case TimeConversionMode::SingleBlock:
    default:
        return {.block = 1, .blockTimeIndex = timeStep + 1, .absoluteTimeIndex = timeStep + 1};
    }
}

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

void addVariableEntries(ISimulationTable& simulationTable,
                        const ILinearProblem& linearProblem,
                        const FillContext& fillContext,
                        const Antares::ModelerStudy::SystemModel::Component& component,
                        unsigned currentBlock,
                        const TimeConversionMode& timeConversionMode,
                        std::optional<unsigned> scenario)
{
    const auto& cid = component.Id();
    const bool isLp = linearProblem.isLP();
    for (const auto& [varName, modelVar]: component.getModel()->Variables())
    {
        bool scenDep = modelVar.IsScenarioDependent();
        bool timeDep = modelVar.isTimeDependent();

        auto handle = [&](std::optional<unsigned> timeStep, std::optional<unsigned> scenIdx)
        {
            std::string fullVarName = Antares::Optimization::VariableDictionary::buildVariableName(
              {cid, varName},
              Antares::Optimization::MCYearAndTime::MCYear{scenIdx.value_or(0)},
              timeStep);
            TimeBlock tb = timeStep ? convertTimeStepToBlockTimeIndex(
                                        *timeStep + fillContext.getGlobalFirstTimeStep(),
                                        timeConversionMode)
                                    : TimeBlock{.block = currentBlock,
                                                .blockTimeIndex = std::nullopt,
                                                .absoluteTimeIndex = std::nullopt};
            auto* var = linearProblem.lookupVariable(fullVarName);
            simulationTable.addEntry(
              {.block = tb.block,
               .component = cid,
               .output = varName,
               .absolute_time_index = tb.absoluteTimeIndex,
               .block_time_index = tb.blockTimeIndex,
               .scenario_index = scenIdx,
               .value = var->solutionValue(),
               .status = isLp ? var->getMipBasisStatus() : MipBasisStatus::NOT_AVAILABLE});
        };

        if (scenDep && timeDep)
        {
            for (unsigned ts = fillContext.getLocalFirstTimeStep();
                 ts <= fillContext.getLocalLastTimeStep();
                 ++ts)
            {
                handle(ts, scenario);
            }
        }
        else if (scenDep)
        {
            handle(std::nullopt, scenario);
        }
        else if (timeDep)
        {
            for (unsigned ts = fillContext.getLocalFirstTimeStep();
                 ts <= fillContext.getLocalLastTimeStep();
                 ++ts)
            {
                handle(ts, std::nullopt);
            }
        }
        else
        {
            handle(std::nullopt, std::nullopt);
        }
    }
}

void addConstraintEntries(ISimulationTable& simulationTable,
                          const ILinearProblem& linearProblem,
                          const FillContext& fillContext,
                          const Antares::ModelerStudy::SystemModel::Component& component,
                          unsigned currentBlock,
                          const TimeConversionMode& timeConversionMode,
                          std::optional<unsigned> scenario)
{
    using TI = Antares::Expressions::Visitors::TimeIndex;
    const auto& cid = component.Id();
    const bool isLp = linearProblem.isLP();
    for (const auto& [cname, modelConstr]: component.getModel()->Constraints())
    {
        TI idxType = Antares::Expressions::Visitors::TimeIndexVisitor(component).dispatch(
          modelConstr.expression().RootNode());

        auto handle = [&](std::optional<unsigned> ts, std::optional<unsigned> scenIdx)
        {
            std::string fullConstName = BuildModelerConstraintName(cid, cname, scenIdx, ts);
            const auto* c = linearProblem.lookupConstraint(fullConstName);
            TimeBlock tb = ts ? convertTimeStepToBlockTimeIndex(
                                  *ts + fillContext.getGlobalFirstTimeStep(),
                                  timeConversionMode)
                              : TimeBlock{.block = currentBlock,
                                          .blockTimeIndex = std::nullopt,
                                          .absoluteTimeIndex = std::nullopt};
            simulationTable.addEntry(
              {.block = tb.block,
               .component = cid,
               .output = cname,
               .absolute_time_index = tb.absoluteTimeIndex,
               .block_time_index = tb.blockTimeIndex,
               .scenario_index = scenIdx,
               .value = std::nullopt,
               .status = isLp ? c->getMipBasisStatus() : MipBasisStatus::NOT_AVAILABLE});
        };

        switch (idxType)
        {
        case TI::VARYING_IN_TIME_AND_SCENARIO:
            for (unsigned ts = fillContext.getLocalFirstTimeStep();
                 ts <= fillContext.getLocalLastTimeStep();
                 ++ts)
            {
                handle(ts, scenario);
            }
            break;
        case TI::VARYING_IN_SCENARIO_ONLY:
            handle(std::nullopt, scenario);
            break;
        case TI::VARYING_IN_TIME_ONLY:
            for (unsigned ts = fillContext.getLocalFirstTimeStep();
                 ts <= fillContext.getLocalLastTimeStep();
                 ++ts)
            {
                handle(ts, std::nullopt);
            }
            break;
        case TI::CONSTANT_IN_TIME_AND_SCENARIO:
        default:
            handle(std::nullopt, std::nullopt);
            break;
        }
    }
}

void addObjectiveValue(ISimulationTable& simulation,
                       double objectiveValue,
                       unsigned int currentBlock,
                       unsigned int scenario)
{
    simulation.addEntry({.block = currentBlock + 1,
                         .component = std::nullopt,
                         .output = "OBJECTIVE_VALUE",
                         .absolute_time_index = std::nullopt,
                         .block_time_index = std::nullopt,
                         .scenario_index = scenario,
                         .value = objectiveValue,
                         .status = MipBasisStatus::NOT_AVAILABLE});
}

void FillSimulationTable(
  ISimulationTable& simulationTable,
  const ILinearProblem& linearProblem,
  double objectiveValue,
  const std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component>& components,
  const FillContext& fillContext,
  unsigned currentBlock,
  const TimeConversionMode& timeConversionMode)
{
    unsigned scenario = fillContext.getYear();
    for (const auto& component: components | std::views::values)
    {
        addVariableEntries(simulationTable,
                           linearProblem,
                           fillContext,
                           component,
                           currentBlock,
                           timeConversionMode,
                           scenario);

        addConstraintEntries(simulationTable,
                             linearProblem,
                             fillContext,
                             component,
                             currentBlock,
                             timeConversionMode,
                             scenario);
    }
    addObjectiveValue(simulationTable, objectiveValue, currentBlock, scenario);
}
