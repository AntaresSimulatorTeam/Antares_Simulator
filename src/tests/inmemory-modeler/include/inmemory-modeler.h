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

#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "antares/expressions/visitors/EvaluationContext.h"
#include "antares/expressions/visitors/TimeIndex.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/VariableDictionary.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/model.h"

#include "inmemory-modeler.h"

namespace Test::Modeler
{
auto build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const Antares::Expressions::Visitors::ParameterType& type = Antares::Expressions::Visitors::
    ParameterType::CONSTANT)
  -> std::pair<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue>;

struct VariableData
{
    std::string id;
    Antares::ModelerStudy::SystemModel::ValueType type;
    Antares::Expressions::Nodes::Node* lb;
    Antares::Expressions::Nodes::Node* ub;
    bool timeDependent = true;
    bool scenarioDependent = true;
};

struct ConstraintData
{
    std::string id;
    Antares::Expressions::Nodes::Node* expression;
};

struct LinearProblemBuildingFixture
{
    std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Model> models;
    Antares::Expressions::Registry<Antares::Expressions::Nodes::Node> nodes;
    std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component> components;
    std::unique_ptr<Antares::Optimisation::LinearProblemApi::ILinearProblem> pb;
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData dummy_data_;

    void createModel(const std::string& modelId,
                     const std::vector<std::string>& parameterIds,
                     const std::vector<VariableData>& variablesData,
                     const std::vector<ConstraintData>& constraintsData,
                     Antares::Expressions::Nodes::Node* objective = nullptr);

    void createModelWithSystemModelParameter(
      const std::string& modelId,
      std::vector<Antares::ModelerStudy::SystemModel::Parameter>,
      const std::vector<VariableData>& variablesData,
      const std::vector<ConstraintData>& constraintsData,
      Antares::Expressions::Nodes::Node* objective = nullptr);

    void createModelWithOneFloatVar(const std::string& modelId,
                                    const std::vector<std::string>& parameterIds,
                                    const std::string& varId,
                                    Antares::Expressions::Nodes::Node* lb,
                                    Antares::Expressions::Nodes::Node* ub,
                                    const std::vector<ConstraintData>& constraintsData,
                                    Antares::Expressions::Nodes::Node* objective = nullptr,
                                    bool time_dependent = false);

    void createComponent(
      const std::string& modelId,
      const std::string& componentId,
      std::map<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue> parameterValues
      = {},
      std::string scenarioGroupId = "");

    Antares::Expressions::Nodes::Node* literal(double value);

    Antares::Expressions::Nodes::Node* parameter(
      const std::string& paramId,
      const Antares::Expressions::Visitors::TimeIndex& timeIndex = Antares::Expressions::Visitors::
        TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    Antares::Expressions::Nodes::Node* variable(
      const std::string& varId,
      const Antares::Expressions::Visitors::TimeIndex& timeIndex = Antares::Expressions::Visitors::
        TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    Antares::Expressions::Nodes::Node* multiply(Antares::Expressions::Nodes::Node* node1,
                                                Antares::Expressions::Nodes::Node* node2);

    Antares::Expressions::Nodes::Node* negate(Antares::Expressions::Nodes::Node* node);

    void buildLinearProblem(
      Antares::Optimisation::LinearProblemApi::FillContext& time_scenario_ctx,
      Antares::Optimisation::LinearProblemDataImpl::LinearProblemData& dummy_data,
      std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IScenario>>& scenarios);

    void buildLinearProblem(
      Antares::Optimisation::LinearProblemApi::FillContext& time_scenario_ctx);

    void buildLinearProblem();
};
} // namespace Test::Modeler
