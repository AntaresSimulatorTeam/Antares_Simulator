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
  Antares::Optimisation::LinearProblemDataImpl::LinearProblemData& dummy_data)
{
    std::vector<std::unique_ptr<Antares::Optimization::ComponentFiller>> fillers;
    std::vector<Antares::Optimisation::LinearProblemApi::LinearProblemFiller*> fillers_ptr;
    // All LP variables coordinates (component id, variable id, scenario, time step)
    Antares::Optimization::VariableDictionary variableDictionary;
    for (auto& component: components)
    {
        auto cf = std::make_unique<Antares::Optimization::ComponentFiller>(component,
                                                                           variableDictionary);
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
    buildLinearProblem(time_scenario_ctx, dummy_data_);
}

void LinearProblemBuildingFixture::buildLinearProblem()
{
    Antares::Optimisation::LinearProblemApi::FillContext time_scenario_ctx = {0, 0};
    buildLinearProblem(time_scenario_ctx);
}

void LinearProblemBuildingFixture::createComponent(
  const std::string& modelId,
  const std::string& componentId,
  std::map<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue> parameterValues)
{
    Antares::ModelerStudy::SystemModel::ComponentBuilder component_builder;
    auto component = component_builder.withId(componentId)
                       .withModel(&models.at(modelId))
                       .withScenarioGroupId("scenario_group")
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

void LinearProblemBuildingFixture::createModel(std::string modelId,
                                               std::vector<std::string> parameterIds,
                                               std::vector<VariableData> variablesData,
                                               std::vector<ConstraintData> constraintsData,
                                               Antares::Expressions::Nodes::Node* objective)
{
    std::vector<Antares::ModelerStudy::SystemModel::Parameter> parameters;
    for (auto parameter_id: std::move(parameterIds))
    {
        parameters.push_back(Antares::ModelerStudy::SystemModel::Parameter(
          parameter_id,
          Antares::ModelerStudy::SystemModel::TimeDependent::NO,
          Antares::ModelerStudy::SystemModel::ScenarioDependent::NO));
    }
    createModelWithSystemModelParameter(std::move(modelId),
                                        parameters,
                                        std::move(variablesData),
                                        std::move(constraintsData),
                                        objective);
}

void LinearProblemBuildingFixture::createModelWithSystemModelParameter(
  std::string modelId,
  std::vector<Antares::ModelerStudy::SystemModel::Parameter> parameters,
  std::vector<VariableData> variablesData,
  std::vector<ConstraintData> constraintsData,
  Antares::Expressions::Nodes::Node* objective)
{
    auto createExpression = [this](Antares::Expressions::Nodes::Node* node)
    {
        Antares::Expressions::NodeRegistry node_registry(node, std::move(nodes));
        Antares::ModelerStudy::SystemModel::Expression expression("expression",
                                                                  std::move(node_registry));
        return expression;
    };

    std::vector<Antares::ModelerStudy::SystemModel::Variable> variables;
    for (auto [id, type, lb, ub, timeDependent, scenarioDependent]: variablesData)
    {
        variables.emplace_back(id,
                               createExpression(lb),
                               createExpression(ub),
                               type,
                               Antares::ModelerStudy::SystemModel::fromBool<
                                 Antares::ModelerStudy::SystemModel::TimeDependent>(timeDependent),
                               Antares::ModelerStudy::SystemModel::fromBool<
                                 Antares::ModelerStudy::SystemModel::ScenarioDependent>(
                                 scenarioDependent));
    }
    std::vector<Antares::ModelerStudy::SystemModel::Constraint> constraints;
    for (auto [id, expression]: constraintsData)
    {
        constraints.push_back(std::move(
          Antares::ModelerStudy::SystemModel::Constraint(id, createExpression(expression))));
    }
    Antares::ModelerStudy::SystemModel::ModelBuilder model_builder;
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
                {{varId,
                  Antares::ModelerStudy::SystemModel::ValueType::FLOAT,
                  lb,
                  ub,
                  time_dependent,
                  false}},
                constraintsData,
                objective);
}
} // namespace Test::Modeler
