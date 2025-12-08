// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#include "inmemory-modeler.h"

#include <map>
#include <memory>

#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/parameter.h"

using namespace Antares::Optimisation;
using namespace Antares::Expressions;

namespace Test::Modeler
{
std::pair<std::string, ParameterTypeAndValue> build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const VariabilityType& type)
{
    return {id, {.id = id, .type = type, .value = value}};
}

void LinearProblemBuildingFixture::buildLinearProblem(
  LinearProblemApi::FillContext& time_scenario_ctx,
  LinearProblemDataImpl::LinearProblemData& dummy_data,
  std::vector<std::unique_ptr<LinearProblemApi::IScenario>>& scenarios)
{
    std::vector<std::unique_ptr<LinearProblemApi::LinearProblemFiller>> fillers;
    // All LP variables coordinates (component id, variable id, scenario, time step)

    for (auto& scenario: scenarios)
    {
        auto name = scenario->group();
        scenarioGroupRepo.addScenario(name, std::move(scenario));
    }
    pb = std::make_unique<LinearProblemMpsolverImpl::OrtoolsLinearProblem>(false, "sirius");
    optimEntityContainer = std::make_unique<OptimEntityContainer>(*pb,
                                                                  &dummy_data,
                                                                  &scenarioGroupRepo);
    optimEntityContainer->addFromSystemComponents(components);
    for (auto& component: components)
    {
        auto cf = std::make_unique<ComponentFiller>(
          component,
          *optimEntityContainer,
          scenarioGroupRepo,
          Antares::Modeler::Config::Location::SUBPROBLEMS);
        fillers.push_back(std::move(cf));
    }
    LinearProblemApi::LinearProblemBuilder linear_problem_builder(fillers);

    linear_problem_builder.build(time_scenario_ctx);
}

void LinearProblemBuildingFixture::buildLinearProblem(
  LinearProblemApi::FillContext& time_scenario_ctx)
{
    std::vector<std::unique_ptr<LinearProblemApi::IScenario>> scenarios;
    buildLinearProblem(time_scenario_ctx, dummy_data_, scenarios);
}

void LinearProblemBuildingFixture::buildLinearProblem()
{
    LinearProblemApi::FillContext time_scenario_ctx = {0, 0, 0, 0, 0};
    buildLinearProblem(time_scenario_ctx);
}

void LinearProblemBuildingFixture::createComponent(
  const std::string& modelId,
  const std::string& componentId,
  std::map<std::string, ParameterTypeAndValue> parameterValues,
  std::string scenarioGroupId)
{
    ComponentBuilder component_builder;
    auto component = component_builder.withId(componentId)
                       .withModel(&models.at(modelId))
                       .withScenarioGroupId(scenarioGroupId)
                       .withParameterValues(std::move(parameterValues))
                       .withIndex(componentIndex_)
                       .build();
    components.emplace_back(component);
    componentIndex_++;
}

Nodes::Node* LinearProblemBuildingFixture::literal(double value)
{
    return nodeRegistry.create<Nodes::LiteralNode>(value);
}

Nodes::Node* LinearProblemBuildingFixture::parameter(const std::string& paramId,
                                                     const VariabilityType& variability)
{
    return nodeRegistry.create<Nodes::ParameterNode>(paramId, variability);
}

Nodes::Node* LinearProblemBuildingFixture::variable(const std::string& varId,
                                                    unsigned index,
                                                    const VariabilityType& variability)
{
    return nodeRegistry.create<Nodes::VariableNode>(varId, index, variability);
}

Nodes::Node* LinearProblemBuildingFixture::multiply(Nodes::Node* node1, Nodes::Node* node2)
{
    return nodeRegistry.create<Nodes::MultiplicationNode>(node1, node2);
}

Nodes::Node* LinearProblemBuildingFixture::negate(Nodes::Node* node)
{
    return nodeRegistry.create<Nodes::NegationNode>(node);
}

Nodes::Node* LinearProblemBuildingFixture::add(Nodes::Node* node1, Nodes::Node* node2)
{
    return nodeRegistry.create<Nodes::SumNode>(node1, node2);
}

void LinearProblemBuildingFixture::createModel(const std::string& modelId,
                                               const std::vector<std::string>& parameterIds,
                                               const std::vector<VariableData>& variablesData,
                                               const std::vector<ConstraintData>& constraintsData,
                                               Nodes::Node* objective)
{
    std::vector<Parameter> parameters;
    for (const auto& parameter_id: std::move(parameterIds))
    {
        parameters.emplace_back(parameter_id, TimeDependent::YES, ScenarioDependent::YES);
    }
    createModelWithSystemModelParameter(modelId,
                                        parameters,
                                        variablesData,
                                        constraintsData,
                                        objective);
}

Expression createExpression(Nodes::Node* node, Registry<Nodes::Node>& nodeRegistry)
{
    NodeRegistry node_registry(node, std::move(nodeRegistry));
    Expression expression("expression", std::move(node_registry));
    return expression;
}

void LinearProblemBuildingFixture::createModelWithSystemModelParameter(
  const std::string& modelId,
  std::vector<Parameter> parameters,
  const std::vector<VariableData>& variablesData,
  const std::vector<ConstraintData>& constraintsData,
  Nodes::Node* objective)
{
    std::vector<Variable> variables;
    for (const auto& [id, type, lb, ub, timeDependent, scenarioDependent]: variablesData)
    {
        variables.emplace_back(id,
                               createExpression(lb, nodeRegistry),
                               createExpression(ub, nodeRegistry),
                               type,
                               fromBool<TimeDependent>(timeDependent),
                               fromBool<ScenarioDependent>(scenarioDependent));
    }
    std::vector<Constraint> constraints;
    for (const auto& [id, expression]: constraintsData)
    {
        constraints.emplace_back(id, createExpression(expression, nodeRegistry));
    }
    ModelBuilder model_builder;
    model_builder.withId(modelId)
      .withParameters(std::move(parameters))
      .withVariables(std::move(variables))
      .withConstraints(std::move(constraints));
    if (objective)
    {
        std::vector<Objective> objectives;
        objectives.emplace_back("objective", createExpression(objective, nodeRegistry));
        model_builder.withObjectives(std::move(objectives));
    }
    auto model = model_builder.build();
    models[modelId] = std::move(model);
}

void LinearProblemBuildingFixture::createModelWithOneFloatVar(
  const std::string& modelId,
  const std::vector<std::string>& parameterIds,
  const std::string& varId,
  Nodes::Node* lb,
  Nodes::Node* ub,
  const std::vector<ConstraintData>& constraintsData,
  Nodes::Node* objective,
  bool time_dependent)
{
    createModel(modelId,
                parameterIds,
                {{varId, ValueType::FLOAT, lb, ub, time_dependent, false}},
                constraintsData,
                objective);
}
} // namespace Test::Modeler
