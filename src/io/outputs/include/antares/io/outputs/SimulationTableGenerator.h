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

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/study/system-model/component.h>
#include "antares/optimisation/linear-problem-api/linearProblem.h"

#include "ISimulationTable.h"

namespace Antares::Modeler
{
struct Data;
}

namespace Antares::Optimisation
{
class OptimEntityContainer;

namespace LinearProblemApi
{
class ILinearProblem;

class IMipSolution;
class FillContext;
} // namespace LinearProblemApi
} // namespace Antares::Optimisation

namespace Antares::Optimization
{
class VariableDictionary;
}

namespace Antares::ModelerStudy::SystemModel
{
class Component;
}

namespace Antares::IO
{
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

Optimisation::VariabilityType updateVariabilityIfShouldForceScenario(
  Optimisation::VariabilityType variability,
  bool forceExportForScenarioIndex);

std::string BuildModelerConstraintName(const std::string& componentId,
                                       const std::string& cname,
                                       const std::optional<unsigned>& ts);

void addVariableEntries(ISimulationTable& simulationTable,
                        const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
                        const Optimisation::LinearProblemApi::FillContext& fillContext,
                        const ModelerStudy::SystemModel::Component& component,
                        const Optimisation::OptimEntityContainer& optimEntityContainer,
                        unsigned currentBlock,
                        const TimeConversionMode& timeConversionMode,
                        std::optional<unsigned> scenario);

void addConstraintEntries(ISimulationTable& simulationTable,
                          const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
                          const Optimisation::LinearProblemApi::FillContext& fillContext,
                          const ModelerStudy::SystemModel::Component& component,
                          const Optimisation::OptimEntityContainer& optimEntityContainer,
                          unsigned currentBlock,
                          const TimeConversionMode& timeConversionMode,
                          std::optional<unsigned> scenario,
                          bool forceExportForScenarioIndex);

void addPortEntries(ISimulationTable& simulationTable,
                    const Optimisation::LinearProblemApi::FillContext& fillContext,
                    const ModelerStudy::SystemModel::Component& component,
                    const Optimisation::OptimEntityContainer& optimEntityContainer,
                    unsigned currentBlock,
                    const TimeConversionMode& timeConversionMode,
                    std::optional<unsigned> scenario,
                    bool forceExportForScenarioIndex);

void addExtraOutputEntries(ISimulationTable& simulationTable,
                           const Optimisation::LinearProblemApi::FillContext& fillContext,
                           const ModelerStudy::SystemModel::Component& component,
                           const Optimisation::OptimEntityContainer& optimEntityContainer,
                           unsigned currentBlock,
                           const TimeConversionMode& timeConversionMode,
                           std::optional<unsigned> scenario,
                           bool forceExportForScenarioIndex);

void addEntriesForNode(ISimulationTable& simulationTable,
                       const Optimisation::LinearProblemApi::FillContext& fillContext,
                       const ModelerStudy::SystemModel::Component& component,
                       const Optimisation::OptimEntityContainer& optimEntityContainer,
                       unsigned currentBlock,
                       const TimeConversionMode& timeConversionMode,
                       std::optional<unsigned> scenario,
                       bool forceExportForScenarioIndex,
                       const std::string& componentId,
                       const std::string& outputName,
                       const Expressions::Nodes::Node* rootNode);
/**
 * Fill modeler outputs in the simulation table
 * @param simulationTable the simulation table to fill
 * @param linearProblem the linear problem containing the optimal solution
 * @param objectiveValue the overall objective value
 * @param modelerData
 * @param optimEntityContainer
 * @param components list of modeler components
 * @param dataSeries the input data series
 * @param fillContext the fill context used to fill the linear problem
 * @param currentBlock the current block that was optimized
 * @param timeConversionMode the block to absolute time conversion mode
 * @param forceExportForScenarioIndex set to true if you want to force the scenario index to be
 * exported for scenario-independent outputs (useful for hybrid mode)
 */
void FillSimulationTable(ISimulationTable& simulationTable,
                         const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
                         double objectiveValue,
                         const Modeler::Data& modelerData,
                         const Optimisation::OptimEntityContainer& optimEntityContainer,
                         const Optimisation::LinearProblemApi::FillContext& fillContext,
                         unsigned currentBlock,
                         const TimeConversionMode& timeConversionMode,
                         bool forceExportForScenarioIndex = false);
} // namespace Antares::IO
