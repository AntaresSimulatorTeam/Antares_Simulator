/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/expressions/nodes/ExpressionsNodes.h"
#include "antares/solver/modeler/api/linearProblemBuilder.h"
#include "antares/solver/modeler/ortoolsImpl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/parameter.h"

using namespace Antares::Solver::Modeler::Api;
using namespace Antares::Study::SystemModel;
using namespace Antares::Optimization;
using namespace Antares::Solver::Nodes;

static Expression generateExpression(Node* node)
{
    // TODO : this seems too complicated; try to make building Expressions easier
    Antares::Solver::Registry<Node> registry;
    Antares::Solver::NodeRegistry node_registry(node, std::move(registry));
    Expression expression("expression", std::move(node_registry));
    return std::move(expression);
}

struct VariableData
{
    std::string id;
    ValueType type;
    Node* lb;
    Node* ub;
};

struct ConstraintData
{
    std::string id;
    Node* expression;
};

struct LinearProblemBuildingFixture
{
    std::map<std::string, Model> models;
    Antares::Solver::Registry<Node> nodes;
    std::vector<Component> components;
    std::unique_ptr<ILinearProblem> pb;

    void createModel(std::string modelId,
                     std::vector<std::string> parameterIds,
                     std::vector<VariableData> variablesData,
                     std::vector<ConstraintData> constraintsData);
    void createModelWithOneFloatVar(const std::string& modelId,
                                    const std::vector<std::string>& parameterIds,
                                    const std::string& varId,
                                    Node* lb,
                                    Node* ub,
                                    const std::vector<ConstraintData>& constraintsData);
    void createComponent(const std::string& modelId,
                         const std::string& componentId,
                         std::map<std::string, double> parameterValues = {});

    Node* literal(double value)
    {
        return nodes.create<LiteralNode>(value);
    }

    Node* parameter(const std::string& paramId)
    {
        return nodes.create<ParameterNode>(paramId);
    }

    void buildLinearProblem();
};

void LinearProblemBuildingFixture::createModel(std::string modelId,
                                               std::vector<std::string> parameterIds,
                                               std::vector<VariableData> variablesData,
                                               std::vector<ConstraintData> constraintsData)
{
    std::vector<Parameter> parameters;
    for (auto parameter_id: parameterIds)
    {
        parameters.push_back(
          Parameter(parameter_id, Parameter::TimeDependent::NO, Parameter::ScenarioDependent::NO));
    }
    std::vector<Variable> variables;
    for (auto [id, type, lb, ub]: variablesData)
    {
        variables.push_back(
          std::move(Variable(id, generateExpression(lb), generateExpression(ub), type)));
    }
    std::vector<Constraint> constraints;
    for (auto [id, expression]: constraintsData)
    {
        constraints.push_back(std::move(Constraint(id, generateExpression(expression))));
    }
    ModelBuilder model_builder;
    auto model = model_builder.withId(modelId)
                   .withParameters(std::move(parameters))
                   .withVariables(std::move(variables))
                   .withConstraints(std::move(constraints))
                   .build();
    models[modelId] = std::move(model);
}

void LinearProblemBuildingFixture::createModelWithOneFloatVar(
  const std::string& modelId,
  const std::vector<std::string>& parameterIds,
  const std::string& varId,
  Node* lb,
  Node* ub,
  const std::vector<ConstraintData>& constraintsData)
{
    createModel(modelId, parameterIds, {{varId, ValueType::FLOAT, lb, ub}}, constraintsData);
}

void LinearProblemBuildingFixture::createComponent(const std::string& modelId,
                                                   const std::string& componentId,
                                                   std::map<std::string, double> parameterValues)
{
    BOOST_CHECK_NO_THROW(models.at(modelId));
    ComponentBuilder component_builder;
    auto component = component_builder.withId(componentId)
                       .withModel(&models.at(modelId))
                       .withScenarioGroupId("scenario_group")
                       .withParameterValues(std::move(parameterValues))
                       .build();
    components.push_back(std::move(component));
}

void LinearProblemBuildingFixture::buildLinearProblem()
{
    std::vector<std::unique_ptr<ComponentFiller>> fillers;
    std::vector<LinearProblemFiller*> fillers_ptr;
    for (auto& component: components)
    {
        auto cf = std::make_unique<ComponentFiller>(component);
        fillers.push_back(std::move(cf));
    }
    for (auto& component_filler: fillers)
    {
        fillers_ptr.push_back(component_filler.get());
    }
    pb = std::make_unique<Antares::Solver::Modeler::OrtoolsImpl::OrtoolsLinearProblem>(false,
                                                                                       "scip");
    LinearProblemBuilder linear_problem_builder(fillers_ptr);
    LinearProblemData dummy_data;
    FillContext dummy_time_scenario_ctx = {0, 0};
    linear_problem_builder.build(*pb.get(), dummy_data, dummy_time_scenario_ctx);
}

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_addVariables_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(var_with_literal_bounds_to_filler__problem_contains_one_var)
{
    createModelWithOneFloatVar("some_model", {}, "var1", literal(-5), literal(10), {});
    createComponent("some_model", "some_component");
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* var = pb->getVariable("some_component.var1");
    BOOST_CHECK(var);
    BOOST_CHECK_EQUAL(var->getLb(), -5);
    BOOST_CHECK_EQUAL(var->getUb(), 10);
    BOOST_CHECK(!var->isInteger());
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 0);
}

BOOST_AUTO_TEST_CASE(var_with_wrong_parameter_lb__exception_is_raised)
{
    createModel("my-model",
                {},
                {{"variable", ValueType::FLOAT, parameter("parameter-not-in-model"), literal(10)}},
                {});
    createComponent("my-model", "my-component");
    // TODO : improve exception message in eval visitor
    BOOST_CHECK_THROW(buildLinearProblem(), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(var_with_wrong_variable_ub__exception_is_raised)
{
    createModel(
      "my-model",
      {},
      {{"variable", ValueType::FLOAT, literal(10), nodes.create<VariableNode>("variable")}},
      {});
    createComponent("my-model", "my-component");
    // TODO : improve exception message in eval visitor
    BOOST_CHECK_THROW(buildLinearProblem(), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(two_variables_given_to_different_fillers__LP_contains_the_two_variables)
{
    createModelWithOneFloatVar("m1", {}, "var1", literal(-1), literal(6), {});
    createModelWithOneFloatVar("m2", {}, "var2", literal(-3), literal(2), {});
    createComponent("m1", "component_1");
    createComponent("m2", "component_2");
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);

    auto* var1 = pb->getVariable("component_1.var1");
    BOOST_CHECK(var1);
    BOOST_CHECK(!var1->isInteger());
    BOOST_CHECK_EQUAL(var1->getLb(), -1.);
    BOOST_CHECK_EQUAL(var1->getUb(), 6.);

    auto* var2 = pb->getVariable("component_2.var2");
    BOOST_CHECK(var2);
    BOOST_CHECK(!var2->isInteger());
    BOOST_CHECK_EQUAL(var2->getLb(), -3.);
    BOOST_CHECK_EQUAL(var2->getUb(), 2.);
}

BOOST_AUTO_TEST_CASE(var_whose_bounds_are_parameters_given_to_component__problem_contains_this_var)
{
    createModel("model",
                {"pmin", "pmax"},
                {{"var1", ValueType::INTEGER, parameter("pmin"), parameter("pmax")}},
                {});
    createComponent("model", "componentToto", {{"pmin", -3.}, {"pmax", 4.}});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* var = pb->getVariable("componentToto.var1");
    BOOST_CHECK(var);
    BOOST_CHECK(var->isInteger());
    BOOST_CHECK_EQUAL(var->getLb(), -3.);
    BOOST_CHECK_EQUAL(var->getUb(), 4.);
}

BOOST_AUTO_TEST_CASE(three_different_vars__exist)
{
    VariableData var1 = {"is_cluster_on", ValueType::BOOL, literal(0), literal(1)};
    VariableData var2 = {"n_started_units", ValueType::INTEGER, literal(0), parameter("nUnits")};
    VariableData var3 = {"p_per_unit", ValueType::FLOAT, parameter("pmin"), parameter("pmax")};
    createModel("thermalClusterModel", {"pmin", "pmax", "nUnits"}, {var1, var2, var3}, {});
    createComponent("thermalClusterModel",
                    "thermalCluster1",
                    {{"pmin", 100.248}, {"pmax", 950.6784}, {"nUnits", 17.}});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 3);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* is_cluster_on = pb->getVariable("thermalCluster1.is_cluster_on");
    BOOST_CHECK(is_cluster_on);
    BOOST_CHECK(is_cluster_on->isInteger());
    BOOST_CHECK_EQUAL(is_cluster_on->getLb(), 0);
    BOOST_CHECK_EQUAL(is_cluster_on->getUb(), 1);
    auto* n_started_units = pb->getVariable("thermalCluster1.n_started_units");
    BOOST_CHECK(n_started_units);
    BOOST_CHECK(n_started_units->isInteger());
    BOOST_CHECK_EQUAL(n_started_units->getLb(), 0);
    BOOST_CHECK_EQUAL(n_started_units->getUb(), 17);
    auto* p_per_unit = pb->getVariable("thermalCluster1.p_per_unit");
    BOOST_CHECK(p_per_unit);
    BOOST_CHECK(!p_per_unit->isInteger());
    BOOST_CHECK_EQUAL(p_per_unit->getLb(), 100.248);
    BOOST_CHECK_EQUAL(p_per_unit->getUb(), 950.6784);
}

BOOST_AUTO_TEST_CASE(one_model_two_components__dont_clash)
{
    createModelWithOneFloatVar("m1", {"ub"}, "var1", literal(-100), parameter("ub"), {});
    createComponent("m1", "component_1", {{"ub", 15}});
    createComponent("m1", "component_2", {{"ub", 48}});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* c1_var1 = pb->getVariable("component_1.var1");
    BOOST_CHECK(c1_var1);
    BOOST_CHECK(!c1_var1->isInteger());
    BOOST_CHECK_EQUAL(c1_var1->getLb(), -100);
    BOOST_CHECK_EQUAL(c1_var1->getUb(), 15);
    auto* c2_var1 = pb->getVariable("component_2.var1");
    BOOST_CHECK(c2_var1);
    BOOST_CHECK(!c2_var1->isInteger());
    BOOST_CHECK_EQUAL(c2_var1->getLb(), -100);
    BOOST_CHECK_EQUAL(c2_var1->getUb(), 48);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_ComponentFiller_addConstraints_)

BOOST_FIXTURE_TEST_CASE(ct_one_var__pb_contains_the_ct, LinearProblemBuildingFixture)
{
    // var1 <= 3
    auto var_node = nodes.create<VariableNode>("var1");
    auto three = literal(3);
    auto ct_node = nodes.create<LessThanOrEqualNode>(var_node, three);
    createModel("model",
                {},
                {{"var1", ValueType::BOOL, literal(-5), literal(10)}},
                {{"ct1", ct_node}});
    createComponent("model", "componentToto");
    buildLinearProblem();

    auto var = pb->getVariable("componentToto.var1");
    BOOST_CHECK(var);
    BOOST_CHECK(var->isInteger());
    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    auto ct = pb->getConstraint("componentToto.ct1");
    BOOST_CHECK(ct);
    BOOST_CHECK_EQUAL(ct->getLb(), -pb->infinity());
    BOOST_CHECK_EQUAL(ct->getUb(), 3);
    BOOST_CHECK_EQUAL(ct->getCoefficient(var), 1);
}

BOOST_FIXTURE_TEST_CASE(ct_one_var_with_coef__pb_contains_the_ct, LinearProblemBuildingFixture)
{
    // 3 * var1 >= 5 * var1 + 5
    // simplified to : -2 * var1 >= 5
    auto var_node = nodes.create<VariableNode>("var__1");
    auto five = literal(5);
    auto coef_node_left = nodes.create<MultiplicationNode>(literal(3), var_node);
    auto coef_node_right = nodes.create<MultiplicationNode>(var_node, five);
    auto sum_node_right = nodes.create<SumNode>(coef_node_right, five);
    auto ct_node = nodes.create<GreaterThanOrEqualNode>(coef_node_left, sum_node_right);

    createModelWithOneFloatVar("model",
                               {},
                               "var__1",
                               literal(-5),
                               literal(10),
                               {{"ct_1", ct_node}});
    createComponent("model", "componentTata");
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_NO_THROW(pb->getVariable("componentTata.var__1"));
    auto var = pb->getVariable("componentTata.var__1");
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    BOOST_CHECK_NO_THROW(pb->getConstraint("componentTata.ct_1"));
    auto ct = pb->getConstraint("componentTata.ct_1");
    BOOST_CHECK(ct);
    BOOST_CHECK_EQUAL(ct->getLb(), 5);
    BOOST_CHECK_EQUAL(ct->getUb(), pb->infinity());
    BOOST_CHECK_EQUAL(ct->getCoefficient(var), -2);
}

// TODO
// - test ct with 2 vars
// - test component with two constraints

BOOST_AUTO_TEST_SUITE_END()
