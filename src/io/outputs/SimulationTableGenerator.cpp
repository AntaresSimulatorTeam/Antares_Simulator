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
#include "antares/io/outputs/SimulationTableGenerator.h"

#include <optional>

#include <boost/test/tools/assertion.hpp>

#include <antares/solver/optim-model-filler/Dimensions.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/expressions/visitors/TimeIndexVisitor.h"
#include "antares/logs/logs.h"
#include "antares/optimisation/linear-problem-api/IScenario.h"
#include "antares/optimisation/linear-problem-api/linearProblem.h"
#include "antares/optimisation/linear-problem-api/mipConstraint.h"
#include "antares/solver/modeler/data.h"
#include "antares/utils/utils.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;
using TI = Antares::Optimisation::TimeIndex;

namespace Antares::IO
{
TimeBlock convertBlockTimeStepToAbsoluteTimeStep(unsigned int timeStep,
                                                 const TimeConversionMode& mode,
                                                 const unsigned currentBlock)
{
    switch (mode)
    {
    case TimeConversionMode::WeeklyBlocks:
        return {.block = currentBlock + 1,
                .blockTimeIndex = timeStep + 1,
                .absoluteTimeIndex = currentBlock * 168 + timeStep + 1};
    case TimeConversionMode::DailyBlocks:
        return {.block = currentBlock + 1,
                .blockTimeIndex = timeStep + 1,
                .absoluteTimeIndex = currentBlock * 24 + timeStep + 1};
    case TimeConversionMode::SingleBlock:
    default:
        return {.block = 1, .blockTimeIndex = timeStep + 1, .absoluteTimeIndex = timeStep + 1};
    }
}

TI updateTimeIndexIfShouldForceScenario(TI timeIndex, bool forceExportForScenarioIndex)
{
    return forceExportForScenarioIndex ? timeIndex | TI::VARYING_IN_SCENARIO_ONLY : timeIndex;
}

std::string BuildModelerConstraintName(const std::string& componentId,
                                       const std::string& cname,
                                       const std::optional<unsigned>& ts)
{
    std::string key = componentId + "." + cname;
    if (ts)
    {
        key += "_" + std::to_string(*ts);
    }
    // ComponentFiller does not yet add the scenario index to the constraint name
    // TODO make this cleaner like with VariableDictionary: maybe read & parse constraint names in
    // LinearProblem instead?
    return key;
}

void addVariableEntries(ISimulationTable& simulationTable,
                        const ILinearProblem& linearProblem,
                        const FillContext& fillContext,
                        const Antares::ModelerStudy::SystemModel::Component& component,
                        const OptimEntityContainer& optimEntityContainer,
                        unsigned currentBlock,
                        const TimeConversionMode& timeConversionMode,
                        std::optional<unsigned> scenario)
{
    const auto& componentId = component.Id();
    const bool isLp = linearProblem.isLP();
    const auto& variables = component.getModel()->Variables();
    for (std::size_t varIndex = 0; varIndex < variables.size(); ++varIndex)
    {
        const auto& modelVar = variables[varIndex];
        bool scenDep = modelVar.IsScenarioDependent();
        bool timeDep = modelVar.isTimeDependent();
        const std::span componentVariables = optimEntityContainer.getComponentVariable(
          component,
          varIndex,
          fillContext.getLocalNumberOfTimeSteps());

        auto handle = [&](std::optional<unsigned> timeStep, std::optional<unsigned> scenIdx)
        {
            TimeBlock tb = timeStep ? convertBlockTimeStepToAbsoluteTimeStep(
                                        *timeStep + fillContext.getGlobalFirstTimeStep(),
                                        timeConversionMode,
                                        currentBlock)
                                    : TimeBlock{.block = currentBlock + 1,
                                                .blockTimeIndex = std::nullopt,
                                                .absoluteTimeIndex = std::nullopt};
            const auto& var = componentVariables[timeStep.value_or(0)];
            simulationTable.addEntry(
              {.block = tb.block,
               .component = componentId,
               .output = modelVar.Id(),
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

void handleDependingOnTimeIndex(
  const FillContext& fillContext,
  std::optional<unsigned> scenario,
  TI idxType,
  const std::function<void(std::optional<unsigned> ts, std::optional<unsigned> scenIdx)>& handle)
{
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

void addConstraintEntries(ISimulationTable& simulationTable,
                          const ILinearProblem& linearProblem,
                          const FillContext& fillContext,
                          const Antares::ModelerStudy::SystemModel::Component& component,
                          const Antares::Optimisation::OptimEntityContainer& optimEntityContainer,
                          unsigned currentBlock,
                          const TimeConversionMode& timeConversionMode,
                          std::optional<unsigned> scenario,
                          bool forceExportForScenarioIndex)
{
    const auto& componentId = component.Id();
    const bool isLp = linearProblem.isLP();

    unsigned constraintLocalIndex = 0;
    for (const auto& modelConstr: component.getModel()->Constraints())
    {
        const auto& constraintId = modelConstr.Id();

        const auto [componentConstraints, timeIndex] = optimEntityContainer.getComponentConstraint(
          component,
          constraintLocalIndex,
          fillContext.getLocalNumberOfTimeSteps());
        ++constraintLocalIndex;

        auto idxType = updateTimeIndexIfShouldForceScenario(timeIndex, forceExportForScenarioIndex);

        auto handle = [&](std::optional<unsigned> ts, std::optional<unsigned> scenIdx)
        {
            const auto& c = componentConstraints[ts.value_or(0)];
            TimeBlock tb = ts ? convertBlockTimeStepToAbsoluteTimeStep(
                                  *ts + fillContext.getGlobalFirstTimeStep(),
                                  timeConversionMode,
                                  currentBlock)
                              : TimeBlock{.block = currentBlock + 1,
                                          .blockTimeIndex = std::nullopt,
                                          .absoluteTimeIndex = std::nullopt};
            simulationTable.addEntry(
              {.block = tb.block,
               .component = componentId,
               .output = constraintId,
               .absolute_time_index = tb.absoluteTimeIndex,
               .block_time_index = tb.blockTimeIndex,
               .scenario_index = scenIdx,
               .value = std::nullopt,
               .status = isLp ? c->getMipBasisStatus() : MipBasisStatus::NOT_AVAILABLE});
        };

        handleDependingOnTimeIndex(fillContext, scenario, idxType, handle);
    }
}

void addObjectiveValue(ISimulationTable& simulation,
                       double objectiveValue,
                       unsigned int currentBlock,
                       unsigned int scenario)
{
    // TODO : handle scenario-independent objectives in full-modeler mode
    simulation.addEntry({.block = currentBlock + 1,
                         .component = std::nullopt,
                         .output = "OBJECTIVE_VALUE",
                         .absolute_time_index = std::nullopt,
                         .block_time_index = std::nullopt,
                         .scenario_index = scenario,
                         .value = objectiveValue,
                         .status = MipBasisStatus::NOT_AVAILABLE});
}

void addEntriesForNode(ISimulationTable& simulationTable,
                       const FillContext& fillContext,
                       const Antares::ModelerStudy::SystemModel::Component& component,
                       const Antares::Optimisation::OptimEntityContainer& optimEntityContainer,
                       unsigned currentBlock,
                       const TimeConversionMode& timeConversionMode,
                       std::optional<unsigned> scenario,
                       bool forceExportForScenarioIndex,
                       const std::string& componentId,
                       const std::string& outputName,
                       const Antares::Expressions::Nodes::Node* rootNode)
{
    auto evalVisitor = Antares::Expressions::Visitors::EvalVisitor(optimEntityContainer,
                                                                   fillContext,
                                                                   component);
    auto value = evalVisitor.dispatch(rootNode);

    TI idxType = Antares::Expressions::Visitors::TimeIndexVisitor(optimEntityContainer, component)
                   .dispatch(rootNode);
    idxType = updateTimeIndexIfShouldForceScenario(idxType, forceExportForScenarioIndex);

    auto handle = [&](std::optional<unsigned> ts, std::optional<unsigned> scenIdx)
    {
        TimeBlock tb = ts ? convertBlockTimeStepToAbsoluteTimeStep(*ts,
                                                                   timeConversionMode,
                                                                   currentBlock)
                          : TimeBlock{.block = currentBlock + 1,
                                      .blockTimeIndex = std::nullopt,
                                      .absoluteTimeIndex = std::nullopt};
        auto val = ts ? value.getValueInVector(ts.value()) : value.valueAsDouble();
        simulationTable.addEntry({.block = tb.block,
                                  .component = componentId,
                                  .output = outputName,
                                  .absolute_time_index = tb.absoluteTimeIndex,
                                  .block_time_index = tb.blockTimeIndex,
                                  .scenario_index = scenIdx,
                                  .value = val,
                                  .status = MipBasisStatus::NOT_AVAILABLE});
    };
    handleDependingOnTimeIndex(fillContext, scenario, idxType, handle);
}

void addPortEntries(ISimulationTable& simulationTable,
                    const FillContext& fillContext,
                    const Antares::ModelerStudy::SystemModel::Component& component,
                    const OptimEntityContainer& optimEntityContainer,
                    unsigned currentBlock,
                    const TimeConversionMode& timeConversionMode,
                    std::optional<unsigned> scenario,
                    bool forceExportForScenarioIndex)
{
    const auto& cid = component.Id();

    for (const auto& [portFieldKey, portFieldDef]: component.getModel()->PortFieldDefinitions())
    {
        Antares::Expressions::Visitors::EvalVisitor evalVisitor(optimEntityContainer,
                                                                fillContext,
                                                                component);

        auto portValue = evalVisitor.dispatch(portFieldDef.Definition().RootNode());

        TI idxType = Antares::Expressions::Visitors::TimeIndexVisitor(optimEntityContainer,
                                                                      component)
                       .dispatch(portFieldDef.Definition().RootNode());
        idxType = updateTimeIndexIfShouldForceScenario(idxType, forceExportForScenarioIndex);
        // TODO: EvalVistior already uses a TimeIndexVisitor under the hood to know if the port
        // is time and/or scenario dependent. It may be more efficient to enrich
        // EvaluationResult
        // by adding a TimeIndex to it? It would require some careful work inside EvalVisitor

        auto handle = [&](std::optional<unsigned> ts, std::optional<unsigned> scenIdx)
        {
            TimeBlock tb = ts ? convertBlockTimeStepToAbsoluteTimeStep(*ts,
                                                                       timeConversionMode,
                                                                       currentBlock)
                              : TimeBlock{.block = currentBlock + 1,
                                          .blockTimeIndex = std::nullopt,
                                          .absoluteTimeIndex = std::nullopt};

            auto value = ts.has_value() ? portValue.valuesAsVector()[ts.value()]
                                        : portValue.valueAsDouble();
            simulationTable.addEntry({.block = tb.block,
                                      .component = cid,
                                      .output = portFieldKey.portId + "." + portFieldKey.fieldId,
                                      .absolute_time_index = tb.absoluteTimeIndex,
                                      .block_time_index = tb.blockTimeIndex,
                                      .scenario_index = scenIdx,
                                      .value = value,
                                      .status = MipBasisStatus::NOT_AVAILABLE});
        };

        handleDependingOnTimeIndex(fillContext, scenario, idxType, handle);
    }
}

void addExtraOutputEntries(ISimulationTable& simulationTable,
                           const FillContext& fillContext,
                           const Antares::ModelerStudy::SystemModel::Component& component,
                           const Antares::Optimisation::OptimEntityContainer& optimEntityContainer,
                           unsigned currentBlock,
                           const TimeConversionMode& timeConversionMode,
                           std::optional<unsigned> scenario,
                           bool forceExportForScenarioIndex)
{
    const auto& componentId = component.Id();
    for (const auto& [extraOutputId, extraOutput]: component.getModel()->ExtraOutputs())
    {
        const auto* rootNode = extraOutput.expression().RootNode();
        std::string outputName = extraOutputId;
        addEntriesForNode(simulationTable,
                          fillContext,
                          component,
                          optimEntityContainer,
                          currentBlock,
                          timeConversionMode,
                          scenario,
                          forceExportForScenarioIndex,
                          componentId,
                          outputName,
                          rootNode);
    }
}

void FillSimulationTable(ISimulationTable& simulationTable,
                         const ILinearProblem& linearProblem,
                         double objectiveValue,
                         const Antares::Modeler::Data& modelerData,
                         const OptimEntityContainer& optimEntityContainer,
                         const FillContext& fillContext,
                         unsigned currentBlock,
                         const TimeConversionMode& timeConversionMode,
                         bool forceExportForScenarioIndex)
{
    Utils::TimeMeasurement measure;
    unsigned scenario = fillContext.getYear();

    for (const auto& component: modelerData.system->Components())
    {
        EmptyScenario emptyScenario;

        addVariableEntries(simulationTable,
                           linearProblem,
                           fillContext,
                           component,
                           optimEntityContainer,
                           currentBlock,
                           timeConversionMode,
                           scenario);

        addConstraintEntries(simulationTable,
                             linearProblem,
                             fillContext,
                             component,
                             optimEntityContainer,
                             currentBlock,
                             timeConversionMode,
                             scenario,
                             forceExportForScenarioIndex);

        addPortEntries(simulationTable,
                       fillContext,
                       component,
                       optimEntityContainer,
                       currentBlock,
                       timeConversionMode,
                       scenario,
                       forceExportForScenarioIndex);

        addExtraOutputEntries(simulationTable,
                              fillContext,
                              component,
                              optimEntityContainer,
                              currentBlock,
                              timeConversionMode,
                              scenario,
                              forceExportForScenarioIndex);
    }
    addObjectiveValue(simulationTable, objectiveValue, currentBlock, scenario);

    measure.tick();
    Antares::logs.info() << "Simulation Table is generated in " << measure.toString();
}
} // namespace Antares::IO
