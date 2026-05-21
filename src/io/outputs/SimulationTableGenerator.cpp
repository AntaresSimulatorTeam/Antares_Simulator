// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/outputs/SimulationTableGenerator.h"

#include <optional>

#include <boost/test/tools/assertion.hpp>

#include <antares/solver/optim-model-filler/Dimensions.h>
#include <antares/solver/optim-model-filler/outOfBoundsTimeShift.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/expressions/visitors/VariabilityVisitor.h"
#include "antares/logs/logs.h"
#include "antares/optimisation/linear-problem-api/linearProblem.h"
#include "antares/optimisation/linear-problem-api/mipConstraint.h"
#include "antares/solver/modeler/ModelerData.h"
#include "antares/utils/utils.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Expressions;

namespace Antares::IO::Outputs
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

VariabilityType updateVariabilityIfShouldForceScenario(VariabilityType variability,
                                                       bool forceExportForScenarioIndex)
{
    return forceExportForScenarioIndex ? variability | VariabilityType::VARYING_IN_SCENARIO_ONLY
                                       : variability;
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

void addVariableEntries(SimulationTable& simulationTable,
                        const ILinearProblem& linearProblem,
                        const FillContext& fillContext,
                        const ModelerStudy::SystemModel::Component& component,
                        const OptimEntityContainer& optimEntityContainer,
                        unsigned currentBlock,
                        const TimeConversionMode& timeConversionMode,
                        unsigned year)
{
    const auto& componentId = component.Id();
    const bool isLp = linearProblem.isLP();
    const auto& variables = component.getModel()->Variables();
    for (std::size_t varIndex = 0; varIndex < variables.size(); ++varIndex)
    {
        const auto& modelVar = variables[varIndex];
        if (modelVar.location() != Solver::Config::Location::SUBPROBLEMS)
        {
            continue;
        }
        bool scenDep = modelVar.IsScenarioDependent();
        bool timeDep = modelVar.isTimeDependent();
        const std::span componentVariables = optimEntityContainer.getComponentVariable(
          component,
          varIndex,
          fillContext.getLocalNumberOfTimeSteps());

        auto handle = [&](std::optional<unsigned> timeStep, unsigned scenIdx)
        {
            TimeBlock tb = timeStep ? convertBlockTimeStepToAbsoluteTimeStep(*timeStep,
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
                handle(ts, year);
            }
        }
        else if (scenDep)
        {
            handle(std::nullopt, year);
        }
        else if (timeDep)
        {
            for (unsigned ts = fillContext.getLocalFirstTimeStep();
                 ts <= fillContext.getLocalLastTimeStep();
                 ++ts)
            {
                handle(ts, year);
            }
        }
        else
        {
            handle(std::nullopt, year);
        }
    }
}

void handleDependingOnVariability(
  const FillContext& fillContext,
  unsigned year,
  VariabilityType variability,
  const std::function<void(std::optional<unsigned> ts, unsigned scenIdx)>& handle)
{
    if (isTimeDependent(variability))
    {
        for (unsigned ts = fillContext.getLocalFirstTimeStep();
             ts <= fillContext.getLocalLastTimeStep();
             ++ts)
        {
            handle(ts, year);
        }
    }
    else
    {
        handle(std::nullopt, year);
    }
}

void addConstraintEntries(SimulationTable& simulationTable,
                          const ILinearProblem& linearProblem,
                          const FillContext& fillContext,
                          const ModelerStudy::SystemModel::Component& component,
                          const OptimEntityContainer& optimEntityContainer,
                          const LinearProblemApi::ILinearProblemData* data,
                          const LinearProblemApi::IScenario& scenario,
                          unsigned currentBlock,
                          const TimeConversionMode& timeConversionMode,
                          unsigned year,
                          bool forceExportForScenarioIndex)
{
    const auto& componentId = component.Id();
    const bool isLp = linearProblem.isLP();

    Expressions::Visitors::EvalVisitor evalVisitor(optimEntityContainer,
                                                   fillContext,
                                                   component,
                                                   data,
                                                   scenario);

    unsigned constraintIndex = 0;
    for (const auto& modelConstr: component.getModel()->Constraints())
    {
        if (modelConstr.location() != Solver::Config::Location::SUBPROBLEMS)
        {
            continue;
        }
        const auto& constraintId = modelConstr.Id();
        auto variability = optimEntityContainer.getConstraintVariability(component,
                                                                         constraintIndex);
        variability = updateVariabilityIfShouldForceScenario(variability,
                                                             forceExportForScenarioIndex);

        const auto componentConstraints = optimEntityContainer.componentConstraints(
          component,
          constraintIndex,
          optimEntityContainer.getConstraintCount(component, constraintIndex));

        std::size_t activeConstraintIndex = 0;
        auto handle = [&](std::optional<unsigned> ts, unsigned scenIdx)
        {
            const IMipConstraint* activeConstraint = nullptr;

            if (!ts.has_value())
            {
                activeConstraint = componentConstraints[0].get();
            }
            else
            {
                // Check if we need to drop the constraint
                if (modelConstr.outOfBoundsProcessingMode()
                      == ModelerStudy::SystemModel::OutOfBoundsProcessingMode::DROP
                    && Optimisation::hasOutOfBoundsTimeShift(modelConstr.expression().RootNode(),
                                                             *ts,
                                                             fillContext,
                                                             evalVisitor))
                {
                    activeConstraint = nullptr;
                }
                else
                {
                    // Increment index and get the constraint
                    activeConstraint = componentConstraints[activeConstraintIndex++].get();
                }
            }

            if (!activeConstraint)
            {
                return;
            }

            TimeBlock tb = ts ? convertBlockTimeStepToAbsoluteTimeStep(*ts,
                                                                       timeConversionMode,
                                                                       currentBlock)
                              : TimeBlock{.block = currentBlock + 1,
                                          .blockTimeIndex = std::nullopt,
                                          .absoluteTimeIndex = std::nullopt};
            simulationTable.addEntry(
              SimulationTableEntry{.block = tb.block,
                                   .component = componentId,
                                   .output = constraintId,
                                   .absolute_time_index = tb.absoluteTimeIndex,
                                   .block_time_index = tb.blockTimeIndex,
                                   .scenario_index = scenIdx,
                                   .value = std::nullopt,
                                   .status = isLp ? activeConstraint->getMipBasisStatus()
                                                  : MipBasisStatus::NOT_AVAILABLE});
        };

        handleDependingOnVariability(fillContext, year, variability, handle);
        ++constraintIndex;
    }
}

void addObjectiveValue(SimulationTable& simulation,
                       double objectiveValue,
                       unsigned currentBlock,
                       unsigned year)
{
    // TODO : handle scenario-independent objectives in full-modeler mode
    simulation.addEntry({.block = currentBlock + 1,
                         .component = std::nullopt,
                         .output = "OBJECTIVE_VALUE",
                         .absolute_time_index = std::nullopt,
                         .block_time_index = std::nullopt,
                         .scenario_index = year,
                         .value = objectiveValue,
                         .status = MipBasisStatus::NOT_AVAILABLE});
}

void addEntriesForNode(SimulationTable& simulationTable,
                       const FillContext& fillContext,
                       Visitors::EvalVisitor& evalVisitor,
                       Visitors::VariabilityVisitor& variabilityVisitor,
                       const ModelerStudy::SystemModel::Component& component,
                       unsigned currentBlock,
                       const TimeConversionMode& timeConversionMode,
                       unsigned year,
                       bool forceExportForScenarioIndex,
                       const std::string& componentId,
                       const std::string& outputName,
                       const Nodes::Node* rootNode)
{
    auto value = evalVisitor.dispatch(rootNode);
    auto variability = variabilityVisitor.dispatch(rootNode);
    variability = updateVariabilityIfShouldForceScenario(variability, forceExportForScenarioIndex);

    auto handle = [&](std::optional<unsigned> ts, unsigned scenIdx)
    {
        if (ts.has_value())
        {
            if (const auto* functionNode = dynamic_cast<const Nodes::FunctionNode*>(rootNode);
                functionNode && functionNode->type() == Nodes::FunctionNodeType::dual)
            {
                const auto* indexNode = dynamic_cast<const Nodes::LiteralNode*>(
                  functionNode->getOperands().at(1));
                const auto constraintIndex = static_cast<std::size_t>(indexNode->value());
                const auto& constraint = component.getModel()->Constraints().at(constraintIndex);

                if (constraint.outOfBoundsProcessingMode()
                      == ModelerStudy::SystemModel::OutOfBoundsProcessingMode::DROP
                    && Optimisation::hasOutOfBoundsTimeShift(constraint.expression().RootNode(),
                                                             *ts,
                                                             fillContext,
                                                             evalVisitor))
                {
                    return;
                }
            }
        }

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
    handleDependingOnVariability(fillContext, year, variability, handle);
}

void addPortEntries(SimulationTable& simulationTable,
                    const FillContext& fillContext,
                    const ModelerStudy::SystemModel::Component& component,
                    Visitors::EvalVisitor& evalVisitor,
                    Visitors::VariabilityVisitor& variabilityVisitor,
                    unsigned currentBlock,
                    const TimeConversionMode& timeConversionMode,
                    unsigned year,
                    bool forceExportForScenarioIndex)
{
    const auto& componentId = component.Id();

    for (const auto& [portFieldKey, portFieldDef]: component.getModel()->PortFieldDefinitions())
    {
        auto portValue = evalVisitor.dispatch(portFieldDef.Definition().RootNode());
        auto variability = variabilityVisitor.dispatch(portFieldDef.Definition().RootNode());

        variability = updateVariabilityIfShouldForceScenario(variability,
                                                             forceExportForScenarioIndex);
        // TODO: EvalVistior already uses a TimeIndexVisitor under the hood to know if the port
        // is time and/or scenario dependent. It may be more efficient to enrich
        // EvaluationResult
        // by adding a TimeIndex to it? It would require some careful work inside EvalVisitor

        auto handle = [&](std::optional<unsigned> ts, unsigned scenIdx)
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
                                      .component = componentId,
                                      .output = portFieldKey.portId + "." + portFieldKey.fieldId,
                                      .absolute_time_index = tb.absoluteTimeIndex,
                                      .block_time_index = tb.blockTimeIndex,
                                      .scenario_index = scenIdx,
                                      .value = value,
                                      .status = MipBasisStatus::NOT_AVAILABLE});
        };

        handleDependingOnVariability(fillContext, year, variability, handle);
    }
}

void addExtraOutputEntries(SimulationTable& simulationTable,
                           const FillContext& fillContext,
                           const ModelerStudy::SystemModel::Component& component,
                           Visitors::EvalVisitor& evalVisitor,
                           Visitors::VariabilityVisitor& variabilityVisitor,
                           unsigned currentBlock,
                           const TimeConversionMode& timeConversionMode,
                           unsigned year,
                           bool forceExportForScenarioIndex)
{
    const auto& componentId = component.Id();
    for (const auto& [extraOutputId, extraOutput]: component.getModel()->ExtraOutputs())
    {
        const auto* rootNode = extraOutput.expression().RootNode();
        std::string outputName = extraOutputId;
        addEntriesForNode(simulationTable,
                          fillContext,
                          evalVisitor,
                          variabilityVisitor,
                          component,
                          currentBlock,
                          timeConversionMode,
                          year,
                          forceExportForScenarioIndex,
                          componentId,
                          outputName,
                          rootNode);
    }
}

void FillSimulationTable(SimulationTable& simulationTable,
                         const ILinearProblem& linearProblem,
                         double objectiveValue,
                         const Solver::ModelerData& modelerData,
                         const OptimEntityContainer& optimContainer,
                         const FillContext& fillContext,
                         unsigned currentBlock,
                         const TimeConversionMode& timeConversionMode,
                         bool forceExportForScenarioIndex)
{
    Utils::TimeMeasurement measure;
    unsigned year = fillContext.getYear();

    for (const auto& component: modelerData.system->Components())
    {
        const auto* data = modelerData.dataSeries.get();
        const auto& scenario = modelerData.scenarioGroupRepository.scenario(
          component.getScenarioGroupId());

        Visitors::EvalVisitor evalVisitor(optimContainer, fillContext, component, data, scenario);
        Visitors::VariabilityVisitor variabilityVisitor(optimContainer, component);

        addVariableEntries(simulationTable,
                           linearProblem,
                           fillContext,
                           component,
                           optimContainer,
                           currentBlock,
                           timeConversionMode,
                           year);

        addConstraintEntries(simulationTable,
                             linearProblem,
                             fillContext,
                             component,
                             optimContainer,
                             data,
                             scenario,
                             currentBlock,
                             timeConversionMode,
                             year,
                             forceExportForScenarioIndex);

        addPortEntries(simulationTable,
                       fillContext,
                       component,
                       evalVisitor,
                       variabilityVisitor,
                       currentBlock,
                       timeConversionMode,
                       year,
                       forceExportForScenarioIndex);

        addExtraOutputEntries(simulationTable,
                              fillContext,
                              component,
                              evalVisitor,
                              variabilityVisitor,
                              currentBlock,
                              timeConversionMode,
                              year,
                              forceExportForScenarioIndex);
    }
    addObjectiveValue(simulationTable, objectiveValue, currentBlock, year);

    measure.tick();
    logs.info() << "Simulation Table is generated in " << measure.toString();
}
} // namespace Antares::IO::Outputs
