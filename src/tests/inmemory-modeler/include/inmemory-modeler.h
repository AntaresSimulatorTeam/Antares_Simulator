// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "antares/expressions/nodes/VariableNode.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/optim-model-filler/Dimensions.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/model.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation;
using namespace Antares::Solver;

namespace Test::Modeler
{
std::pair<std::string, ParameterTypeAndValue> build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const VariabilityType& type = VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

struct VariableData
{
    std::string id;
    ValueType type;
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
    std::unordered_map<std::string, Model> models;
    Antares::Expressions::Registry<Antares::Expressions::Nodes::Node> nodeRegistry;
    std::unique_ptr<Antares::Optimisation::LinearProblemApi::ILinearProblem> pb;
    std::vector<Component> components;
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData dummy_data_;
    ModelerData modelerData;
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo;
    std::unique_ptr<Antares::Optimisation::OptimEntityContainer> optimEntityContainer;

    void createModel(const std::string& modelId,
                     const std::vector<std::string>& parameterIds,
                     const std::vector<VariableData>& variablesData,
                     const std::vector<ConstraintData>& constraintsData,
                     Antares::Expressions::Nodes::Node* objective = nullptr);

    void createModelWithSystemModelParameter(
      const std::string& modelId,
      std::vector<Parameter>,
      const std::vector<VariableData>& variablesData,
      const std::vector<ConstraintData>& constraintsData,
      Antares::Expressions::Nodes::Node* objective = nullptr);

    void createModelWithMultipleObjectives(
      const std::string& modelId,
      std::vector<Parameter>,
      const std::vector<VariableData>& variablesData,
      const std::vector<ConstraintData>& constraintsData,
      std::vector<Antares::Expressions::Nodes::Node*> objectives);

    void createModelWithOneFloatVar(const std::string& modelId,
                                    const std::vector<std::string>& parameterIds,
                                    const std::string& varId,
                                    Antares::Expressions::Nodes::Node* lb,
                                    Antares::Expressions::Nodes::Node* ub,
                                    const std::vector<ConstraintData>& constraintsData,
                                    Antares::Expressions::Nodes::Node* objective = nullptr,
                                    bool time_dependent = false);

    void createComponent(const std::string& modelId,
                         const std::string& componentId,
                         std::map<std::string, ParameterTypeAndValue> parameterValues = {},
                         std::string scenarioGroupId = "");

    Antares::Expressions::Nodes::Node* literal(double value);

    Antares::Expressions::Nodes::Node* parameter(
      const std::string& paramId,
      const Antares::Optimisation::VariabilityType& variability = Antares::Optimisation::
        VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    Antares::Expressions::Nodes::Node* variable(
      const std::string& varId,
      unsigned int index,
      const Antares::Optimisation::VariabilityType& variability = Antares::Optimisation::
        VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    Antares::Expressions::Nodes::Node* multiply(Antares::Expressions::Nodes::Node* node1,
                                                Antares::Expressions::Nodes::Node* node2);
    Antares::Expressions::Nodes::Node* add(Antares::Expressions::Nodes::Node* node1,
                                           Antares::Expressions::Nodes::Node* node2);
    Antares::Expressions::Nodes::Node* Sum(Antares::Expressions::Nodes::Node* node);

    Antares::Expressions::Nodes::Node* negate(Antares::Expressions::Nodes::Node* node);

    void buildLinearProblem(
      Antares::Optimisation::LinearProblemApi::FillContext& time_scenario_ctx,
      Antares::Optimisation::LinearProblemDataImpl::LinearProblemData& dummy_data,
      std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IScenario>>& scenarios);

    void buildLinearProblem(
      Antares::Optimisation::LinearProblemApi::FillContext& time_scenario_ctx);

    void buildLinearProblem();

    ModelerData& getModelerData()
    {
        SystemBuilder systemBuilder;
        auto system = systemBuilder.withId("system").withComponents(std::move(components)).build();
        modelerData.system = std::make_unique<System>(std::move(system));
        modelerData.dataSeries = std::make_unique<
          Antares::Optimisation::LinearProblemDataImpl::LinearProblemData>(std::move(dummy_data_));
        modelerData.scenarioGroupRepository = std::move(scenarioGroupRepo);
        return modelerData;
    }

private:
    int componentIndex_ = 0;
};
} // namespace Test::Modeler
