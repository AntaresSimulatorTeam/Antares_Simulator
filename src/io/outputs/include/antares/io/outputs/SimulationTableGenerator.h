// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <string>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/study/system-model/component.h>
#include "antares/optimisation/linear-problem-api/linearProblem.h"

#include "SimulationTable.h"

namespace Antares::Solver
{
struct ModelerData;
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

namespace Antares::IO::Outputs
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

void addVariableEntries(SimulationTable& simulationTable,
                        const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
                        const Optimisation::LinearProblemApi::FillContext& fillContext,
                        const ModelerStudy::SystemModel::Component& component,
                        const Optimisation::OptimEntityContainer& optimEntityContainer,
                        unsigned currentBlock,
                        const TimeConversionMode& timeConversionMode,
                        unsigned year);

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
void FillSimulationTable(SimulationTable& simulationTable,
                         const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
                         double objectiveValue,
                         const Solver::ModelerData& modelerData,
                         const Optimisation::OptimEntityContainer& optimEntityContainer,
                         const Optimisation::LinearProblemApi::FillContext& fillContext,
                         unsigned currentBlock,
                         const TimeConversionMode& timeConversionMode,
                         bool forceExportForScenarioIndex = false);
} // namespace Antares::IO::Outputs
