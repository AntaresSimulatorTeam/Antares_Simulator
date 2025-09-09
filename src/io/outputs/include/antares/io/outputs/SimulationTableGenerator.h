/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include "antares/optimisation/linear-problem-api/linearProblem.h"
#include "antares/solver/modeler/data.h"
#include "antares/solver/optim-model-filler/EvaluationContextProvider.h"
#include "antares/solver/optim-model-filler/VariableDictionary.h"

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

TimeBlock convertBlockTimeStepToAbsoluteTimeStep(unsigned int timeStep,
                                                 const TimeConversionMode& mode,
                                                 unsigned currentBlock);

Antares::Expressions::Visitors::TimeIndex updateTimeIndexIfShouldForceScenario(
  Antares::Expressions::Visitors::TimeIndex timeIndex,
  bool forceExportForScenarioIndex);

std::string BuildModelerConstraintName(const std::string& cid,
                                       const std::string& cname,
                                       const std::optional<unsigned>& ts);

void addVariableEntries(
  ISimulationTable& simulationTable,
  const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
  const Antares::ModelerStudy::SystemModel::Component& component,
  unsigned currentBlock,
  const TimeConversionMode& timeConversionMode,
  std::optional<unsigned> scenario);

void addConstraintEntries(
  ISimulationTable& simulationTable,
  const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
  const Antares::ModelerStudy::SystemModel::Component& component,
  unsigned currentBlock,
  const TimeConversionMode& timeConversionMode,
  std::optional<unsigned> scenario,
  bool forceExportForScenarioIndex,
  const Antares::Optimisation::EvaluationContextProvider& contextProvider);

void addPortEntries(ISimulationTable& simulationTable,
                    const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
                    const Antares::ModelerStudy::SystemModel::Component& component,
                    unsigned currentBlock,
                    const TimeConversionMode& timeConversionMode,
                    std::optional<unsigned> scenario,
                    bool forceExportForScenarioIndex,
                    const Antares::Optimisation::EvaluationContextProvider& contextProvider);

/**
 * Fill modeler outputs in the simulation table
 * @param simulationTable the simulation table to fill
 * @param linearProblem the linear problem containing the optimal solution
 * @param objectiveValue the overall objective value
 * @param components list of modeler components
 * @param dataSeries the input data series
 * @param fillContext the fill context used to fill the linear problem
 * @param currentBlock the current block that was optimized
 * @param timeConversionMode the block to absolute time conversion mode
 * @param forceExportForScenarioIndex set to true if you want to force the scenario index to be
 * exported for scenario-independent outputs (useful for hybrid mode)
 */
void FillSimulationTable(
  ISimulationTable& simulationTable,
  const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
  double objectiveValue,
  const Antares::Modeler::Data& modelerData,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
  unsigned currentBlock,
  const TimeConversionMode& timeConversionMode,
  bool forceExportForScenarioIndex = false);
