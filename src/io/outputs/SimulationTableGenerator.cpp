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
#include "antares/expressions/visitors/EvalVisitorPostOptim.h"
#include "antares/expressions/visitors/TimeIndexVisitor.h"
#include "antares/optimisation/linear-problem-api/IScenario.h"
#include "antares/optimisation/linear-problem-api/linearProblem.h"
#include "antares/optimisation/linear-problem-api/mipConstraint.h"
#include "antares/solver/optim-model-filler/EvaluationContextProvider.h"

#include "../../modeler/FileWriter.h"

using namespace Antares::Optimisation::LinearProblemApi;
using TI = Antares::Expressions::Visitors::TimeIndex;

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
                        unsigned currentBlock,
                        const TimeConversionMode& timeConversionMode,
                        std::optional<unsigned> scenario)
{
    const auto& componentId = component.Id();
    const bool isLp = linearProblem.isLP();
    for (const auto& [varName, modelVar]: component.getModel()->Variables())
    {
        bool scenDep = modelVar.IsScenarioDependent();
        bool timeDep = modelVar.isTimeDependent();

        auto handle = [&](std::optional<unsigned> timeStep, std::optional<unsigned> scenIdx)
        {
            std::string fullVarName = Antares::Optimization::VariableDictionary::buildVariableName(
              {componentId, varName},
              Antares::Optimization::MCYearAndTime::MCYear{scenIdx.value_or(0)},
              timeStep);

            TimeBlock tb = timeStep ? convertBlockTimeStepToAbsoluteTimeStep(
                                        *timeStep + fillContext.getGlobalFirstTimeStep(),
                                        timeConversionMode,
                                        currentBlock)
                                    : TimeBlock{.block = currentBlock + 1,
                                                .blockTimeIndex = std::nullopt,
                                                .absoluteTimeIndex = std::nullopt};
            auto* var = linearProblem.lookupVariable(fullVarName);
            simulationTable.addEntry(
              {.block = tb.block,
               .component = componentId,
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
                          std::optional<unsigned> scenario,
                          bool forceExportForScenarioIndex,
                          const Antares::Optimisation::EvaluationContextProvider& contextProvider)
{
    const auto& componentId = component.Id();
    const bool isLp = linearProblem.isLP();
    for (const auto& [cname, modelConstr]: component.getModel()->Constraints())
    {
        TI idxType = Antares::Expressions::Visitors::TimeIndexVisitor(component, contextProvider)
                       .dispatch(modelConstr.expression().RootNode());
        idxType = updateTimeIndexIfShouldForceScenario(idxType, forceExportForScenarioIndex);

        scenario = isScenarioDependant(idxType) ? scenario : std::nullopt;

        if (isTimeDependant(idxType))
        {
            for (unsigned ts = fillContext.getLocalFirstTimeStep();
                 ts <= fillContext.getLocalLastTimeStep();
                 ++ts)
            {
                std::string fullConstName = BuildModelerConstraintName(componentId, cname, ts);
                const auto* c = linearProblem.lookupConstraint(fullConstName);
                TimeBlock tb = convertBlockTimeStepToAbsoluteTimeStep(
                  ts + fillContext.getGlobalFirstTimeStep(),
                  timeConversionMode,
                  currentBlock);

                simulationTable.addEntry(
                  {.block = tb.block,
                   .component = componentId,
                   .output = cname,
                   .absolute_time_index = tb.absoluteTimeIndex,
                   .block_time_index = tb.blockTimeIndex,
                   .scenario_index = scenario,
                   .value = std::nullopt,
                   .status = isLp ? c->getMipBasisStatus() : MipBasisStatus::NOT_AVAILABLE});
            }
        }
        else
        {
            std::string fullConstName = BuildModelerConstraintName(componentId,
                                                                   cname,
                                                                   std::nullopt);
            const auto* c = linearProblem.lookupConstraint(fullConstName);
            simulationTable.addEntry(
              {.block = currentBlock + 1,
               .component = componentId,
               .output = cname,
               .absolute_time_index = std::nullopt,
               .block_time_index = std::nullopt,
               .scenario_index = scenario,
               .value = std::nullopt,
               .status = isLp ? c->getMipBasisStatus() : MipBasisStatus::NOT_AVAILABLE});
        }
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
                       unsigned currentBlock,
                       const TimeConversionMode& timeConversionMode,
                       std::optional<unsigned> scenario,
                       bool forceExportForScenarioIndex,
                       const Antares::Optimisation::EvaluationContextProvider& contextProvider,
                       const std::string& componentId,
                       const std::string& outputName,
                       const Antares::Expressions::Nodes::Node* rootNode)
{
    Antares::Expressions::Visitors::EvalVisitorPostOptim evalVisitor(contextProvider,
                                                                     fillContext,
                                                                     &component);
    auto value = evalVisitor.dispatch(rootNode);

    TI idxType = Antares::Expressions::Visitors::TimeIndexVisitor(component, contextProvider)
                   .dispatch(rootNode);

    idxType = updateTimeIndexIfShouldForceScenario(idxType, forceExportForScenarioIndex);

    scenario = isScenarioDependant(idxType) ? scenario : std::nullopt;

    if (isTimeDependant(idxType))
    {
        for (unsigned ts = fillContext.getLocalFirstTimeStep();
             ts <= fillContext.getLocalLastTimeStep();
             ++ts)
        {
            TimeBlock tb = convertBlockTimeStepToAbsoluteTimeStep(ts,
                                                                  timeConversionMode,
                                                                  currentBlock);
            auto val = value.valuesAsVector()[ts];
            simulationTable.addEntry({.block = tb.block,
                                      .component = componentId,
                                      .output = outputName,
                                      .absolute_time_index = tb.absoluteTimeIndex,
                                      .block_time_index = tb.blockTimeIndex,
                                      .scenario_index = scenario,
                                      .value = val,
                                      .status = MipBasisStatus::NOT_AVAILABLE});
        }
    }
    else
    {
        double val = value.valueAsDouble();
        simulationTable.addEntry({.block = currentBlock + 1,
                                  .component = componentId,
                                  .output = outputName,
                                  .absolute_time_index = std::nullopt,
                                  .block_time_index = std::nullopt,
                                  .scenario_index = scenario,
                                  .value = val,
                                  .status = MipBasisStatus::NOT_AVAILABLE});
    }
}

void addPortEntries(ISimulationTable& simulationTable,
                    const FillContext& fillContext,
                    const Antares::ModelerStudy::SystemModel::Component& component,
                    unsigned currentBlock,
                    const TimeConversionMode& timeConversionMode,
                    std::optional<unsigned> scenario,
                    bool forceExportForScenarioIndex,
                    const Antares::Optimisation::EvaluationContextProvider& contextProvider)
{
    const auto& componentId = component.Id();
    for (const auto& [portFieldKey, portFieldDef]: component.getModel()->PortFieldDefinitions())
    {
        const auto& rootNode = portFieldDef.Definition().RootNode();
        std::string outputName = portFieldKey.portId + "." + portFieldKey.fieldId;
        addEntriesForNode(simulationTable,
                          fillContext,
                          component,
                          currentBlock,
                          timeConversionMode,
                          scenario,
                          forceExportForScenarioIndex,
                          contextProvider,
                          componentId,
                          outputName,
                          rootNode);
    }
}

void addExtraOutputEntries(ISimulationTable& simulationTable,
                           const FillContext& fillContext,
                           const Antares::ModelerStudy::SystemModel::Component& component,
                           unsigned currentBlock,
                           const TimeConversionMode& timeConversionMode,
                           std::optional<unsigned> scenario,
                           bool forceExportForScenarioIndex,
                           const Antares::Optimisation::EvaluationContextProvider& contextProvider)
{
    const auto& componentId = component.Id();
    for (const auto& [extraOutputId, extraOutput]: component.getModel()->ExtraOutputs())
    {
        const auto& rootNode = extraOutput.expression().RootNode();
        std::string outputName = extraOutputId;
        addEntriesForNode(simulationTable,
                          fillContext,
                          component,
                          currentBlock,
                          timeConversionMode,
                          scenario,
                          forceExportForScenarioIndex,
                          contextProvider,
                          componentId,
                          outputName,
                          rootNode);
    }
}

void FillSimulationTable(ISimulationTable& simulationTable,
                         const ILinearProblem& linearProblem,
                         double objectiveValue,
                         const Antares::Modeler::Data& modelerData,
                         const FillContext& fillContext,
                         unsigned currentBlock,
                         const TimeConversionMode& timeConversionMode,
                         bool forceExportForScenarioIndex)
{
    unsigned scenario = fillContext.getYear();
    std::map<std::string, double> solutions;
    for (int i = 0; i < linearProblem.variableCount(); ++i)
    {
        auto* var = linearProblem.getVariable(i);
        solutions.try_emplace(var->getName(), var->solutionValue());
    }
    for (const auto& component: modelerData.system->Components() | std::views::values)
    {
        EmptyScenario emptyScenario;
        Antares::Optimisation::EvaluationContextProvider
          contextProvider(*modelerData.dataSeries, modelerData.scenarioGroupRepository, solutions);

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
                             scenario,
                             forceExportForScenarioIndex,
                             contextProvider);

        addPortEntries(simulationTable,
                       fillContext,
                       component,
                       currentBlock,
                       timeConversionMode,
                       scenario,
                       forceExportForScenarioIndex,
                       contextProvider);

        addExtraOutputEntries(simulationTable,
                              fillContext,
                              component,
                              currentBlock,
                              timeConversionMode,
                              scenario,
                              forceExportForScenarioIndex,
                              contextProvider);
    }
    addObjectiveValue(simulationTable, objectiveValue, currentBlock, scenario);
}
