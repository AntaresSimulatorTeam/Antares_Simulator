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
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/solver/optim-model-filler/scenarioGroupRepo.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/parameter.h"

using namespace Antares::ModelerStudy::SystemModel;

namespace Test::Modeler
{
std::pair<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue>
build_context_parameter_with(const std::string& id,
                             const std::string& value,
                             const Antares::Expressions::Visitors::ParameterType& type)
{
    return {id, {.id = id, .type = type, .value = value}};
}

void LinearProblemBuildingFixture::buildLinearProblem(
  Antares::Optimisation::LinearProblemApi::FillContext& time_scenario_ctx,
  Antares::Optimisation::LinearProblemDataImpl::LinearProblemData& dummy_data,
  std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IScenario>>& scenarios)
{
    std::vector<std::unique_ptr<Antares::Optimisation::ComponentFiller>> fillers;
    std::vector<Antares::Optimisation::LinearProblemApi::LinearProblemFiller*> fillers_ptr;
    // All LP variables coordinates (component id, variable id, scenario, time step)
    Antares::Optimization::VariableDictionary variableDictionary;
    Antares::Optimisation::ScenarioGroupRepository scenario_group_repository;
    for (auto& scenario: scenarios)
    {
        auto name = scenario->group();
        scenario_group_repository.addScenario(name, std::move(scenario));
    }
    for (auto& component: components)
    {
        auto cf = std::make_unique<Antares::Optimisation::ComponentFiller>(
          component,
          variableDictionary,
          scenario_group_repository);
        fillers.push_back(std::move(cf));
    }
    for (auto& component_filler: fillers)
    {
        fillers_ptr.push_back(component_filler.get());
    }
    pb = std::make_unique<Antares::Optimisation::LinearProblemMpsolverImpl::OrtoolsLinearProblem>(
      false,
      "sirius");
    Antares::Optimisation::LinearProblemApi::LinearProblemBuilder linear_problem_builder(
      fillers_ptr);

    linear_problem_builder.build(*pb, dummy_data, time_scenario_ctx);
}

void LinearProblemBuildingFixture::buildLinearProblem(
  Antares::Optimisation::LinearProblemApi::FillContext& time_scenario_ctx)
{
    std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IScenario>> scenarios;
    buildLinearProblem(time_scenario_ctx, dummy_data_, scenarios);
}

void LinearProblemBuildingFixture::buildLinearProblem()
{
    Antares::Optimisation::LinearProblemApi::FillContext time_scenario_ctx = {0, 0, 0};
    buildLinearProblem(time_scenario_ctx);
}

void LinearProblemBuildingFixture::createComponent(
  const std::string& modelId,
  const std::string& componentId,
  std::map<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue> parameterValues,
  std::string scenarioGroupId)
{
    ComponentBuilder component_builder;
    auto component = component_builder.withId(componentId)
                       .withModel(&models.at(modelId))
                       .withScenarioGroupId(scenarioGroupId)
                       .withParameterValues(move(parameterValues))
                       .build();
    components.push_back(std::move(component));
}

Antares::Expressions::Nodes::Node* LinearProblemBuildingFixture::literal(double value)
{
    return nodes.create<Antares::Expressions::Nodes::LiteralNode>(value);
}

Antares::Expressions::Nodes::Node* LinearProblemBuildingFixture::parameter(
  const std::string& paramId,
  const Antares::Expressions::Visitors::TimeIndex& timeIndex)
{
    return nodes.create<Antares::Expressions::Nodes::ParameterNode>(paramId, timeIndex);
}

Antares::Expressions::Nodes::Node* LinearProblemBuildingFixture::variable(
  const std::string& varId,
  const Antares::Expressions::Visitors::TimeIndex& timeIndex)
{
    return nodes.create<Antares::Expressions::Nodes::VariableNode>(varId, timeIndex);
}

Antares::Expressions::Nodes::Node* LinearProblemBuildingFixture::multiply(
  Antares::Expressions::Nodes::Node* node1,
  Antares::Expressions::Nodes::Node* node2)
{
    return nodes.create<Antares::Expressions::Nodes::MultiplicationNode>(node1, node2);
}

Antares::Expressions::Nodes::Node* LinearProblemBuildingFixture::negate(
  Antares::Expressions::Nodes::Node* node)
{
    return nodes.create<Antares::Expressions::Nodes::NegationNode>(node);
}

void LinearProblemBuildingFixture::createModel(const std::string& modelId,
                                               const std::vector<std::string>& parameterIds,
                                               const std::vector<VariableData>& variablesData,
                                               const std::vector<ConstraintData>& constraintsData,
                                               Antares::Expressions::Nodes::Node* objective)
{
    std::vector<Parameter> parameters;
    for (const auto& parameter_id: std::move(parameterIds))
    {
        parameters.emplace_back(parameter_id, TimeDependent::NO, ScenarioDependent::NO);
    }
    createModelWithSystemModelParameter(modelId,
                                        parameters,
                                        variablesData,
                                        constraintsData,
                                        objective);
}

void LinearProblemBuildingFixture::createModelWithSystemModelParameter(
  const std::string& modelId,
  std::vector<Parameter> parameters,
  const std::vector<VariableData>& variablesData,
  const std::vector<ConstraintData>& constraintsData,
  Antares::Expressions::Nodes::Node* objective)
{
    auto createExpression = [this](Antares::Expressions::Nodes::Node* node)
    {
        Antares::Expressions::NodeRegistry node_registry(node, std::move(nodes));
        Expression expression("expression", std::move(node_registry));
        return expression;
    };

    std::vector<Variable> variables;
    for (const auto& [id, type, lb, ub, timeDependent, scenarioDependent]: variablesData)
    {
        variables.emplace_back(
          id,
          createExpression(lb),
          createExpression(ub),
          type,
          Antares::ModelerStudy::SystemModel::fromBool<TimeDependent>(timeDependent),
          Antares::ModelerStudy::SystemModel::fromBool<ScenarioDependent>(scenarioDependent));
    }
    std::vector<Constraint> constraints;
    for (const auto& [id, expression]: constraintsData)
    {
        constraints.push_back(std::move(Constraint(id, createExpression(expression))));
    }
    ModelBuilder model_builder;
    model_builder.withId(modelId)
      .withParameters(std::move(parameters))
      .withVariables(std::move(variables))
      .withConstraints(std::move(constraints));
    if (objective)
    {
        model_builder.withObjective(createExpression(objective));
    }
    auto model = model_builder.build();
    models[modelId] = std::move(model);
}

void LinearProblemBuildingFixture::createModelWithOneFloatVar(
  const std::string& modelId,
  const std::vector<std::string>& parameterIds,
  const std::string& varId,
  Antares::Expressions::Nodes::Node* lb,
  Antares::Expressions::Nodes::Node* ub,
  const std::vector<ConstraintData>& constraintsData,
  Antares::Expressions::Nodes::Node* objective,
  bool time_dependent)
{
    createModel(modelId,
                parameterIds,
                {{varId, ValueType::FLOAT, lb, ub, time_dependent, false}},
                constraintsData,
                objective);
}
} // namespace Test::Modeler
