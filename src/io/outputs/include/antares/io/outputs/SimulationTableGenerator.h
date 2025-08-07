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
#pragma once
#include <string>
#include <unordered_map>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/study/system-model/component.h>
#include "antares/expressions/visitors/TimeIndexVisitor.h"

#include "ISimulationTable.h"

namespace Antares::Optimisation::LinearProblemApi
{
class ILinearProblem;

class IMipSolution;
class FillContext;
} // namespace Antares::Optimisation::LinearProblemApi

namespace Antares::Optimization
{
class VariableDictionary;
}

namespace Antares::ModelerStudy::SystemModel
{
class Component;
}

struct TimeBlock
{
    unsigned int block;
    std::optional<int> blockTimeIndex;
    std::optional<int> absoluteTimeIndex;
};
enum class TimeConversionMode
{
    SingleBlock, // for Modeler
    DailyBlocks,
    WeeklyBlocks
};

TimeBlock convertTimeStepToBlockTimeIndex(unsigned int timeStep, const TimeConversionMode& mode);

template<typename SolverTraits, typename VarLookup>
void addVariableEntries(ISimulationTable& simulationTable,
                        const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
                        const Antares::ModelerStudy::SystemModel::Component& component,
                        VarLookup variableLookup,
                        unsigned currentBlock,
                        const TimeConversionMode& timeConversionMode,
                        std::optional<unsigned> scenario,
                        bool isLp)
{
    const auto& cid = component.Id();

    for (const auto& [varName, modelVar]: component.getModel()->Variables())
    {
        bool scenDep = modelVar.IsScenarioDependent();
        bool timeDep = modelVar.isTimeDependent();

        auto handle = [&](std::optional<unsigned> timeStep, std::optional<unsigned> scenIdx)
        {
            const auto* var = variableLookup(cid, varName, scenIdx, timeStep);
            TimeBlock tb = timeStep ? convertTimeStepToBlockTimeIndex(*timeStep, timeConversionMode)
                                    : TimeBlock{.block = currentBlock,
                                                .blockTimeIndex = std::nullopt,
                                                .absoluteTimeIndex = std::nullopt};
            simulationTable.addEntry({.block = tb.block,
                                      .component = cid,
                                      .output = varName,
                                      .absolute_time_index = tb.absoluteTimeIndex,
                                      .block_time_index = tb.blockTimeIndex,
                                      .scenario_index = scenIdx,
                                      .value = SolverTraits::getValue(var),
                                      .status = isLp ? SolverTraits::getStatus(var)
                                                     : Antares::Optimisation::LinearProblemApi::
                                                         MipBasisStatus::NOT_AVAILABLE});
        };

        if (scenDep && timeDep)
        {
            for (unsigned ts = fillContext.getFirstTimeStep(); ts <= fillContext.getLastTimeStep();
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
            for (unsigned ts = fillContext.getFirstTimeStep(); ts <= fillContext.getLastTimeStep();
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

template<typename SolverTraits, typename ConstraintLookup>
void addConstraintEntries(ISimulationTable& simulationTable,
                          const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
                          const Antares::ModelerStudy::SystemModel::Component& component,
                          ConstraintLookup lookupConstraint,
                          unsigned currentBlock,
                          const TimeConversionMode& timeConversionMode,
                          std::optional<unsigned> scenario,
                          bool isLp)
{
    using TI = Antares::Expressions::Visitors::TimeIndex;
    const auto& cid = component.Id();

    for (const auto& [cname, modelConstr]: component.getModel()->Constraints())
    {
        TI idxType = Antares::Expressions::Visitors::TimeIndexVisitor(component).dispatch(
          modelConstr.expression().RootNode());

        auto handle = [&](std::optional<unsigned> ts, std::optional<unsigned> scenIdx)
        {
            const auto* c = lookupConstraint(cid, cname, scenIdx, ts);
            TimeBlock tb = ts ? convertTimeStepToBlockTimeIndex(*ts, timeConversionMode)
                              : TimeBlock{.block = currentBlock,
                                          .blockTimeIndex = std::nullopt,
                                          .absoluteTimeIndex = std::nullopt};
            simulationTable.addEntry({.block = tb.block,
                                      .component = cid,
                                      .output = cname,
                                      .absolute_time_index = tb.absoluteTimeIndex,
                                      .block_time_index = tb.blockTimeIndex,
                                      .scenario_index = scenIdx,
                                      .value = std::nullopt,
                                      .status = isLp ? SolverTraits::getStatus(c)
                                                     : Antares::Optimisation::LinearProblemApi::
                                                         MipBasisStatus::NOT_AVAILABLE});
        };

        switch (idxType)
        {
        case TI::VARYING_IN_TIME_AND_SCENARIO:
            for (unsigned ts = fillContext.getFirstTimeStep(); ts <= fillContext.getLastTimeStep();
                 ++ts)
            {
                handle(ts, scenario);
            }
            break;
        case TI::VARYING_IN_SCENARIO_ONLY:
            handle(std::nullopt, scenario);
            break;
        case TI::VARYING_IN_TIME_ONLY:
            for (unsigned ts = fillContext.getFirstTimeStep(); ts <= fillContext.getLastTimeStep();
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

std::string BuildModelerConstraintName(const std::string& cid,
                                       const std::string& cname,
                                       const std::optional<unsigned>& scen,
                                       const std::optional<unsigned>& ts);
void FillSimulationTable(
  ISimulationTable& simulationTable,
  const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
  const Antares::Optimisation::LinearProblemApi::IMipSolution& solution,
  const std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component>& components,
  const Antares::Optimization::VariableDictionary& variableDictionary,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext);
