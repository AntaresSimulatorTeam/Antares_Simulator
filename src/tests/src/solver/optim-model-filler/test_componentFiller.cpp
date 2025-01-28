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
#include "antares/solver/expressions/visitors/TimeIndex.h"
#include "antares/solver/modeler/api/linearProblemBuilder.h"
#include "antares/solver/modeler/ortoolsImpl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/parameter.h"
#include "antares/study/system-model/timeAndScenarioType.h"

#include "unit_test_utils.h"

using namespace Antares::Solver::Modeler::Api;
using namespace Antares::Study::SystemModel;
using namespace Antares::Optimization;
using namespace Antares::Solver::Nodes;
using namespace std;

struct VariableData
{
    string id;
    ValueType type;
    Node* lb;
    Node* ub;
    bool timeDependent = true;
    bool scenarioDependent = true;
};

struct ConstraintData
{
    string id;
    Node* expression;
};

struct LinearProblemBuildingFixture
{
    map<string, Model> models;
    Antares::Solver::Registry<Node> nodes;
    vector<Component> components;
    unique_ptr<ILinearProblem> pb;

    void createModel(string modelId,
                     vector<string> parameterIds,
                     vector<VariableData> variablesData,
                     vector<ConstraintData> constraintsData,
                     Node* objective = nullptr);

    void createModelWithOneFloatVar(const string& modelId,
                                    const vector<string>& parameterIds,
                                    const string& varId,
                                    Node* lb,
                                    Node* ub,
                                    const vector<ConstraintData>& constraintsData,
                                    Node* objective = nullptr,
                                    bool time_dependent = false)
    {
        createModel(modelId,
                    parameterIds,
                    {{varId, ValueType::FLOAT, lb, ub, time_dependent, false}},
                    constraintsData,
                    objective);
    }

    void createComponent(const string& modelId,
                         const string& componentId,
                         map<string, double> parameterValues = {});

    Node* literal(double value)
    {
        return nodes.create<LiteralNode>(value);
    }

    Node* parameter(const string& paramId,
                    const Antares::Solver::Visitors::TimeIndex& timeIndex = Antares::Solver::
                      Visitors::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        return nodes.create<ParameterNode>(paramId, timeIndex);
    }

    Node* variable(const string& varId,
                   const Antares::Solver::Visitors::TimeIndex& timeIndex = Antares::Solver::
                     Visitors::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        return nodes.create<VariableNode>(varId, timeIndex);
    }

    Node* multiply(Node* node1, Node* node2)
    {
        return nodes.create<MultiplicationNode>(node1, node2);
    }

    Node* negate(Node* node)
    {
        return nodes.create<NegationNode>(node);
    }

    void buildLinearProblem(FillContext& time_scenario_ctx);

    void buildLinearProblem()
    {
        FillContext time_scenario_ctx = {0, 0};
        buildLinearProblem(time_scenario_ctx);
    }
};

void LinearProblemBuildingFixture::createModel(string modelId,
                                               vector<string> parameterIds,
                                               vector<VariableData> variablesData,
                                               vector<ConstraintData> constraintsData,
                                               Node* objective)
{
    auto createExpression = [this](Node* node)
    {
        Antares::Solver::NodeRegistry node_registry(node, move(nodes));
        Expression expression("expression", move(node_registry));
        return expression;
    };

    vector<Parameter> parameters;
    for (auto parameter_id: parameterIds)
    {
        parameters.push_back(Parameter(parameter_id, TimeDependent::NO, ScenarioDependent::NO));
    }
    vector<Variable> variables;
    for (auto [id, type, lb, ub, timeDependent, scenarioDependent]: variablesData)
    {
        variables.push_back(move(Variable(id,
                                          createExpression(lb),
                                          createExpression(ub),
                                          type,
                                          fromBool<TimeDependent>(timeDependent),
                                          fromBool<ScenarioDependent>(scenarioDependent))));
    }
    vector<Constraint> constraints;
    for (auto [id, expression]: constraintsData)
    {
        constraints.push_back(move(Constraint(id, createExpression(expression))));
    }
    ModelBuilder model_builder;
    model_builder.withId(modelId)
      .withParameters(move(parameters))
      .withVariables(move(variables))
      .withConstraints(move(constraints));
    if (objective)
    {
        model_builder.withObjective(createExpression(objective));
    }
    auto model = model_builder.build();
    models[modelId] = move(model);
}

void LinearProblemBuildingFixture::createComponent(const string& modelId,
                                                   const string& componentId,
                                                   map<string, double> parameterValues)
{
    BOOST_CHECK_NO_THROW(models.at(modelId));
    ComponentBuilder component_builder;
    auto component = component_builder.withId(componentId)
                       .withModel(&models.at(modelId))
                       .withScenarioGroupId("scenario_group")
                       .withParameterValues(move(parameterValues))
                       .build();
    components.push_back(move(component));
}

void LinearProblemBuildingFixture::buildLinearProblem(FillContext& time_scenario_ctx)
{
    vector<unique_ptr<ComponentFiller>> fillers;
    vector<LinearProblemFiller*> fillers_ptr;
    for (auto& component: components)
    {
        auto cf = make_unique<ComponentFiller>(component);
        fillers.push_back(move(cf));
    }
    for (auto& component_filler: fillers)
    {
        fillers_ptr.push_back(component_filler.get());
    }
    pb = make_unique<Antares::Solver::Modeler::OrtoolsImpl::OrtoolsLinearProblem>(false, "sirius");
    LinearProblemBuilder linear_problem_builder(fillers_ptr);
    LinearProblemData dummy_data;

    linear_problem_builder.build(*pb, dummy_data, time_scenario_ctx);
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

BOOST_AUTO_TEST_CASE(ten_timesteps_var_with_literal_bounds_to_filler__problem_contains_ten_vars)
{
    createModelWithOneFloatVar("some_model",
                               {},
                               "var1",
                               literal(-5),
                               literal(10),
                               {},
                               nullptr,
                               true);
    createComponent("some_model", "some_component");
    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getNumberOfTimestep(); // = 10
    BOOST_CHECK_EQUAL(pb->variableCount(), nb_var);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    for (unsigned int i = 0; i < nb_var; i++)
    {
        auto* var = pb->getVariable("some_component.var1_" + to_string(i));
        BOOST_REQUIRE(var);
        BOOST_CHECK_EQUAL(var->getLb(), -5);
        BOOST_CHECK_EQUAL(var->getUb(), 10);
        BOOST_CHECK(!var->isInteger());
        BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 0);
    }
}

BOOST_AUTO_TEST_CASE(var_with_wrong_parameter_lb__exception_is_raised)
{
    createModel("my-model",
                {},
                {{"variable", ValueType::FLOAT, parameter("parameter-not-in-model"), literal(10)}},
                {});
    createComponent("my-model", "my-component");
    // TODO : improve exception message in eval visitor
    BOOST_CHECK_THROW(buildLinearProblem(), out_of_range);
}

BOOST_AUTO_TEST_CASE(var_with_wrong_variable_ub__exception_is_raised)
{
    createModel("my-model",
                {},
                {{"variable", ValueType::FLOAT, literal(10), variable("variable")}},
                {});
    createComponent("my-model", "my-component");
    // TODO : improve exception message in eval visitor
    BOOST_CHECK_THROW(buildLinearProblem(), out_of_range);
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

BOOST_AUTO_TEST_CASE(
  two_times_10_variables_given_to_different_fillers__LP_contains_the_two_variables)
{
    createModelWithOneFloatVar("m1", {}, "var1", literal(-1), literal(6), {}, nullptr, true);
    createModelWithOneFloatVar("m2", {}, "var2", literal(-3), literal(2), {}, nullptr, true);
    createComponent("m1", "component_1");
    createComponent("m2", "component_2");
    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getNumberOfTimestep(); // = 10

    BOOST_CHECK_EQUAL(pb->variableCount(), 2 * 10);
    for (auto i = 0; i < nb_var; i++)
    {
        auto* var1 = pb->getVariable("component_1.var1_" + to_string(i));
        BOOST_REQUIRE(var1);
        BOOST_CHECK(!var1->isInteger());
        BOOST_CHECK_EQUAL(var1->getLb(), -1.);
        BOOST_CHECK_EQUAL(var1->getUb(), 6.);

        auto* var2 = pb->getVariable("component_2.var2_" + to_string(i));
        BOOST_REQUIRE(var2);
        BOOST_CHECK(!var2->isInteger());
        BOOST_CHECK_EQUAL(var2->getLb(), -3.);
        BOOST_CHECK_EQUAL(var2->getUb(), 2.);
    }
}

BOOST_AUTO_TEST_CASE(var_whose_bounds_are_parameters_given_to_component__problem_contains_this_var)
{
    createModel("model",
                {"pmin", "pmax"},
                {{"var1", ValueType::INTEGER, parameter("pmin"), parameter("pmax"), false, false}},
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
    VariableData var1 = {"is_cluster_on", ValueType::BOOL, literal(0), literal(1), false, false};
    VariableData var2 = {"n_started_units",
                         ValueType::INTEGER,
                         literal(0),
                         parameter("nUnits"),
                         false,
                         false};
    VariableData var3 = {"p_per_unit",
                         ValueType::FLOAT,
                         parameter("pmin"),
                         parameter("pmax"),
                         false,
                         false};
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

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_addConstraints_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(ct_one_var__pb_contains_the_ct)
{
    // var1 <= 3
    auto var_node = variable("var1");
    auto three = literal(3);
    auto ct_node = nodes.create<LessThanOrEqualNode>(var_node, three);

    createModel("model",
                {},
                {{"var1", ValueType::BOOL, literal(-5), literal(10), false, false}},
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

BOOST_AUTO_TEST_CASE(ct_with_ten_vars__pb_contains_ten_ct)
{
    // var1 <= 3
    auto var_node = variable("var1", Antares::Solver::Visitors::TimeIndex::VARYING_IN_TIME_ONLY);
    auto three = literal(3);
    auto ct_node = nodes.create<LessThanOrEqualNode>(var_node, three);

    createModel("model",
                {},
                {{"var1", ValueType::BOOL, literal(-5), literal(10), true, false}},
                {{"ct1", ct_node}});
    createComponent("model", "componentToto");
    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getNumberOfTimestep(); // = 10

    BOOST_CHECK_EQUAL(pb->variableCount(), 10);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 10);

    for (auto i = 0; i < nb_var; i++)
    {
        auto ct = pb->getConstraint("componentToto.ct1_" + to_string(i));
        BOOST_REQUIRE(ct);
        BOOST_CHECK_EQUAL(ct->getLb(), -pb->infinity());
        BOOST_CHECK_EQUAL(ct->getUb(), 3);
        auto var = pb->getVariable("componentToto.var1_" + to_string(i));
        BOOST_REQUIRE(var);
        BOOST_CHECK(var->isInteger());
        BOOST_CHECK_EQUAL(ct->getCoefficient(var), 1);
    }
}

BOOST_AUTO_TEST_CASE(ct_one_var_with_coef__pb_contains_the_ct)
{
    // 3 * var1 >= 5 * var1 + 5
    // simplified to : -2 * var1 >= 5
    auto var_node = variable("var__1");
    auto five = literal(5);
    auto three = literal(3);
    auto coef_node_left = multiply(three, var_node);
    auto coef_node_right = multiply(var_node, five);
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

BOOST_AUTO_TEST_CASE(ct_with_two_vars)
{
    // param1(-16) * v1 + 8 * v2 + 5 - param2(8) = 7 * v1 +  param3(5) * v2 + 89 + 5 * param4(-3)
    // simplifies to: -23 * v1 + 3 * v2 = 77
    vector<string> params = {"param1", "param2", "param3", "param4"};
    auto threeHundreds = literal(300.);
    auto minus50 = literal(-50.);
    VariableData var1Data = {"v1", ValueType::FLOAT, minus50, threeHundreds, false, false};
    auto sixty = literal(60.);
    auto seventy5 = literal(75.);
    VariableData var2Data = {"v2", ValueType::FLOAT, sixty, seventy5, false, false};

    auto v1 = variable("v1");
    auto param1 = parameter("param1");
    auto v2 = variable("v2");
    auto param2 = parameter("param2");
    auto sum_node_left = nodes.create<SumNode>(multiply(v1, param1),
                                               multiply(literal(8), v2),
                                               literal(5),
                                               negate(param2));
    auto param3 = parameter("param3");
    auto param4 = parameter("param4");
    auto sum_node_right = nodes.create<SumNode>(multiply(v1, literal(7)),
                                                multiply(param3, v2),
                                                literal(89),
                                                multiply(literal(5), param4));
    auto ct_node = nodes.create<EqualNode>(sum_node_left, sum_node_right);

    createModel("my_new_model", params, {var1Data, var2Data}, {{"constraint1", ct_node}});
    createComponent("my_new_model",
                    "my_component",
                    {{"param1", -16}, {"param2", 8}, {"param3", 5}, {"param4", -3}});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    BOOST_CHECK_NO_THROW(pb->getConstraint("my_component.constraint1"));
    auto ct = pb->getConstraint("my_component.constraint1");
    BOOST_CHECK(ct);
    BOOST_CHECK_EQUAL(ct->getLb(), 77);
    BOOST_CHECK_EQUAL(ct->getUb(), 77);
    BOOST_CHECK_EQUAL(ct->getCoefficient(pb->getVariable("my_component.v1")), -23);
    BOOST_CHECK_EQUAL(ct->getCoefficient(pb->getVariable("my_component.v2")), 3);
}

BOOST_AUTO_TEST_CASE(two_constraints__they_are_created)
{
    auto minus50 = literal(-50.);
    auto threeHundred = literal(300.);
    // 3 * v1 -2 <= v2 (simplifies to : 3 * v1 - 2 * v2 <= 2)
    // v2 <= v1 / 2 (simplifies to : -0.5 * v1 + v2 <= 0)
    VariableData var1Data = {"v1", ValueType::FLOAT, minus50, threeHundred, false, false};
    auto sixty = literal(60.);
    auto seventy5 = literal(75.);
    VariableData var2Data = {"v2", ValueType::FLOAT, sixty, seventy5, false, false};

    auto v1 = variable("v1");
    auto three = literal(3);
    auto two_1 = literal(2);
    auto v2 = variable("v2");
    auto ct1_node = nodes.create<LessThanOrEqualNode>(
      nodes.create<SubtractionNode>(multiply(three, v1), two_1),
      v2);
    auto two_2 = literal(2);
    auto ct2_node = nodes.create<LessThanOrEqualNode>(v2, nodes.create<DivisionNode>(v1, two_2));

    createModel("my_new_model", {}, {var1Data, var2Data}, {{"ct1", ct1_node}, {"ct2", ct2_node}});
    createComponent("my_new_model", "my_component", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 2);

    BOOST_CHECK_NO_THROW(pb->getConstraint("my_component.ct1"));
    auto ct1 = pb->getConstraint("my_component.ct1");
    BOOST_CHECK(ct1);
    BOOST_CHECK_EQUAL(ct1->getLb(), -numeric_limits<float>::infinity());
    BOOST_CHECK_EQUAL(ct1->getUb(), 2);
    BOOST_CHECK_EQUAL(ct1->getCoefficient(pb->getVariable("my_component.v1")), 3);
    BOOST_CHECK_EQUAL(ct1->getCoefficient(pb->getVariable("my_component.v2")), -1);

    BOOST_CHECK_NO_THROW(pb->getConstraint("my_component.ct2"));
    auto ct2 = pb->getConstraint("my_component.ct2");
    BOOST_CHECK(ct2);
    BOOST_CHECK_EQUAL(ct2->getLb(), -numeric_limits<float>::infinity());
    BOOST_CHECK_EQUAL(ct2->getUb(), 0);
    BOOST_CHECK_EQUAL(ct2->getCoefficient(pb->getVariable("my_component.v1")), -0.5);
    BOOST_CHECK_EQUAL(ct2->getCoefficient(pb->getVariable("my_component.v2")), 1);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_addObjective_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(one_var_with_objective)
{
    auto objective = variable("x");

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_NO_THROW(pb->getVariable("componentA.x"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->getVariable("componentA.x")), 1);
}

BOOST_AUTO_TEST_CASE(one_time_dependent_var_with_objective)
{
    auto objective = variable("x", Antares::Solver::Visitors::TimeIndex::VARYING_IN_TIME_ONLY);

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective, true);
    createComponent("model", "componentA", {});

    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getNumberOfTimestep(); // = 10

    BOOST_CHECK_EQUAL(pb->variableCount(), nb_var);
    for (auto i = 0; i < nb_var; i++)
    {
        const auto var_name = "componentA.x_" + to_string(i);
        BOOST_CHECK_NO_THROW(pb->getVariable(var_name));
        BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->getVariable(var_name)), 1);
    }
}

BOOST_AUTO_TEST_CASE(two_vars_but_only_one_in_objective)
{
    VariableData var1Data = {"v1", ValueType::FLOAT, literal(-50.), literal(300.), false, false};
    VariableData var2Data = {"v2", ValueType::FLOAT, literal(60.), literal(75.), false, false};
    auto objective = multiply(variable("v2"), literal(37));

    createModel("model", {}, {var1Data, var2Data}, {}, objective);
    createComponent("model", "componentA", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_NO_THROW(pb->getVariable("componentA.v1"));
    BOOST_CHECK_NO_THROW(pb->getVariable("componentA.v2"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->getVariable("componentA.v1")), 0);
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->getVariable("componentA.v2")), 37);
}

BOOST_AUTO_TEST_CASE(one_var_with_param_objective)
{
    // -param(5)*param(5) * x
    auto objective = multiply(negate(multiply(parameter("param"), parameter("param"))),
                              variable("x"));
    createModelWithOneFloatVar("model", {"param"}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {{"param", 5}});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_NO_THROW(pb->getVariable("componentA.x"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->getVariable("componentA.x")), -25);
}

BOOST_AUTO_TEST_CASE(offset_in_objective__throws_exception)
{
    auto objective = literal(6);
    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {});
    BOOST_CHECK_EXCEPTION(buildLinearProblem(),
                          invalid_argument,
                          checkMessage("Antares does not support objective offsets (found in model "
                                       "'model' of component 'componentA')."));
}

BOOST_AUTO_TEST_SUITE_END()
