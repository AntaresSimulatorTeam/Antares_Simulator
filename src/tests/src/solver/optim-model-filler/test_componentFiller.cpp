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

#include "antares/solver/expressions/nodes/GreaterThanOrEqualNode.h"
#include "antares/solver/expressions/nodes/LessThanOrEqualNode.h"
#include "antares/solver/expressions/nodes/LiteralNode.h"
#include "antares/solver/expressions/nodes/MultiplicationNode.h"
#include "antares/solver/expressions/nodes/ParameterNode.h"
#include "antares/solver/expressions/nodes/SumNode.h"
#include "antares/solver/expressions/nodes/VariableNode.h"
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

struct FloatVariableData
{
    std::string name;
    double lowerBound = 0;
    double upperBound = 0;
};

struct LinearProblemBuildingFixture
{
    ModelBuilder model_builder;
    ComponentBuilder component_builder;
    std::map<std::string, Model> models;
    Antares::Solver::Registry<Node> node_memory;

    Component makeComponent(std::string modelId, std::string componentId);
    LinearProblemBuildingFixture();
    Model createModelWithOneVarAndLiteralBounds(std::string id,
                                                std::string var_id,
                                                double lb,
                                                double ub);
};

LinearProblemBuildingFixture::LinearProblemBuildingFixture()
{
    auto model1 = createModelWithOneVarAndLiteralBounds("oneVar_lb-1_ub6", "var1", -1, 6);
    auto model2 = createModelWithOneVarAndLiteralBounds("oneVar_lb-3_ub2", "var2", -3, 2);
    models["oneVar_lb-1_ub6"] = std::move(model1);
    models["oneVar_lb-3_ub2"] = std::move(model2);
}

Model LinearProblemBuildingFixture::createModelWithOneVarAndLiteralBounds(std::string id,
                                                                          std::string var_id,
                                                                          double lb,
                                                                          double ub)
{
    auto lb_node = node_memory.create<LiteralNode>(lb);
    auto ub_node = node_memory.create<LiteralNode>(ub);
    Variable variable(var_id,
                      generateExpression(lb_node),
                      generateExpression(ub_node),
                      ValueType::FLOAT);
    std::vector<Variable> variables;
    variables.push_back(std::move(variable));
    auto model = model_builder.withId(id).withVariables(std::move(variables)).build();
    return std::move(model);
}

Component LinearProblemBuildingFixture::makeComponent(const std::string modelId,
                                                      const std::string componentId)
{
    auto component = component_builder.withId(componentId)
                       .withModel(&models[modelId])
                       .withScenarioGroupId("scenario_group")
                       .build();
    return std::move(component);
}

static std::unique_ptr<ILinearProblem> buildProblem(std::vector<LinearProblemFiller*> fillers)
{
    std::unique_ptr<ILinearProblem>
      pb = std::make_unique<Antares::Solver::Modeler::OrtoolsImpl::OrtoolsLinearProblem>(false,
                                                                                         "scip");
    LinearProblemBuilder linear_problem_builder(fillers);
    LinearProblemData dummy_data;
    FillContext dummy_context = {0, 0};
    linear_problem_builder.build(*pb.get(), dummy_data, dummy_context);
    return std::move(pb);
}

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_addVariables_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(var_with_literal_bounds_to_filler__problem_contains_one_var)
{
    LiteralNode lb_node(-5);
    LiteralNode ub_node(10);
    Variable var1 = {"var1",
                     generateExpression(&lb_node),
                     generateExpression(&ub_node),
                     ValueType::FLOAT};
    std::vector<Variable> variables;
    variables.push_back(std::move(var1));
    auto model = model_builder.withId("model").withVariables(std::move(variables)).build();

    auto component = component_builder.withId("componentToto")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();

    auto filler = std::make_unique<ComponentFiller>(component);
    auto pb = buildProblem({filler.get()});

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* var = pb->getVariable("componentToto.var1");
    BOOST_CHECK(var);
    BOOST_CHECK_EQUAL(var->getLb(), -5);
    BOOST_CHECK_EQUAL(var->getUb(), 10);
    // TODO : check variable type (float, not integer)
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 0);
}

BOOST_AUTO_TEST_CASE(var_with_wrong_parameter_lb__exception_is_raised)
{
    ParameterNode lb_node("this-parameter-does-not-exist-in-model");
    LiteralNode ub_node(10);
    Variable var1 = {"var1",
                     generateExpression(&lb_node),
                     generateExpression(&ub_node),
                     ValueType::FLOAT};
    std::vector<Variable> variables;
    variables.push_back(std::move(var1));
    auto model = model_builder.withId("model").withVariables(std::move(variables)).build();

    auto component = component_builder.withId("componentToto")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();

    auto filler = std::make_unique<ComponentFiller>(component);
    // TODO : improve exception message
    BOOST_CHECK_THROW(buildProblem({filler.get()}), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(var_with_wrong_variable_ub__exception_is_raised)
{
    LiteralNode lb_node(10);
    VariableNode ub_node("var1");
    Variable var1 = {"var1",
                     generateExpression(&lb_node),
                     generateExpression(&ub_node),
                     ValueType::FLOAT};
    std::vector<Variable> variables;
    variables.push_back(std::move(var1));
    auto model = model_builder.withId("model").withVariables(std::move(variables)).build();

    auto component = component_builder.withId("componentToto")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();

    auto filler = std::make_unique<ComponentFiller>(component);
    // TODO : improve exception message
    BOOST_CHECK_THROW(buildProblem({filler.get()}), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(two_variables_given_to_different_fillers__LP_contains_the_two_variables)
{
    auto component1 = makeComponent("oneVar_lb-1_ub6", "component_1");
    auto component2 = makeComponent("oneVar_lb-3_ub2", "component_2");

    auto filler1 = std::make_unique<ComponentFiller>(component1);
    auto filler2 = std::make_unique<ComponentFiller>(component2);

    auto pb = buildProblem({filler1.get(), filler2.get()});

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);

    auto* var1 = pb->getVariable("component_1.var1");
    BOOST_CHECK(var1);
    BOOST_CHECK_EQUAL(var1->getLb(), -1.);
    BOOST_CHECK_EQUAL(var1->getUb(), 6.);

    auto* var2 = pb->getVariable("component_2.var2");
    BOOST_CHECK(var2);
    BOOST_CHECK_EQUAL(var2->getLb(), -3.);
    BOOST_CHECK_EQUAL(var2->getUb(), 2.);
}

BOOST_AUTO_TEST_CASE(var_whose_bounds_are_parameters_given_to_component__problem_contains_this_var)
{
    ParameterNode lb_node("pmin");
    ParameterNode ub_node("pmax");

    Variable var1 = {"var1",
                     generateExpression(&lb_node),
                     generateExpression(&ub_node),
                     ValueType::FLOAT};
    std::vector<Variable> variables;
    variables.push_back(std::move(var1));

    Parameter pmin("pmin", Parameter::TimeDependent::NO, Parameter::ScenarioDependent::NO);
    Parameter pmax("pmax", Parameter::TimeDependent::NO, Parameter::ScenarioDependent::NO);
    std::vector<Parameter> parameters = {std::move(pmin), std::move(pmax)};

    auto model = model_builder.withId("model")
                   .withVariables(std::move(variables))
                   .withParameters(std::move(parameters))
                   .build();

    auto component = component_builder.withId("componentToto")
                       .withModel(&model)
                       .withParameterValues({{"pmin", -3.}, {"pmax", 4.}})
                       .withScenarioGroupId("scenario_group")
                       .build();

    auto filler = std::make_unique<ComponentFiller>(component);

    auto pb = buildProblem({filler.get()});

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* var = pb->getVariable("componentToto.var1");
    BOOST_CHECK(var);
    BOOST_CHECK_EQUAL(var->getLb(), -3.);
    BOOST_CHECK_EQUAL(var->getUb(), 4.);
}

// TODO
// - test with 3 variables (different types: float, bool, int)
// - test with one model, 1 variable, 2 components (and 2 component fillers)
// - test with one model, 1 variable, lb and ub are dependent on component parameters (2 components)
//        in model builder : .withParameters({"p_min", NO, NO})
//        in component builder : use withParameterValues

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_ComponentFiller_addConstraints_)

BOOST_FIXTURE_TEST_CASE(ct_one_var__pb_contains_the_ct, LinearProblemBuildingFixture)
{
    LiteralNode var_lb_node(-5);
    LiteralNode var_ub_node(10);
    Variable var1 = {"var1",
                     generateExpression(&var_lb_node),
                     generateExpression(&var_ub_node),
                     ValueType::FLOAT};
    std::vector<Variable> variables;
    variables.push_back(std::move(var1));

    auto var_node = VariableNode("var1");
    auto three = LiteralNode(3);
    auto ct_node = LessThanOrEqualNode(&var_node, &three);
    // TODO: replace with parsing of "var1 <= 3" ?
    Constraint constraint = {"ct1", generateExpression(&ct_node)};
    std::vector<Constraint> vec_cts;
    vec_cts.push_back(std::move(constraint));

    auto model = model_builder.withId("model")
                   .withVariables(std::move(variables))
                   .withConstraints(std::move(vec_cts))
                   .build();

    auto component = component_builder.withId("componentToto")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();

    auto filler = std::make_unique<ComponentFiller>(component);
    auto pb = buildProblem({filler.get()});

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    auto ct = pb->getConstraint("componentToto.ct1");
    BOOST_CHECK(ct);
    BOOST_CHECK_EQUAL(ct->getLb(), -pb->infinity());
    BOOST_CHECK_EQUAL(ct->getUb(), 3);
    BOOST_CHECK_EQUAL(ct->getCoefficient(pb->getVariable("componentToto.var1")), 1);
}

BOOST_FIXTURE_TEST_CASE(ct_one_var_with_coef__pb_contains_the_ct, LinearProblemBuildingFixture)
{
    LiteralNode var_lb_node(-5);
    LiteralNode var_ub_node(10);
    Variable var1 = {"var1",
                     generateExpression(&var_lb_node),
                     generateExpression(&var_ub_node),
                     ValueType::FLOAT};
    std::vector<Variable> variables;
    variables.push_back(std::move(var1));

    // 3 * var1 >= 5 * var1 + 5 => -2 * var1 >= 5
    auto var_node = VariableNode("var1");
    auto three = LiteralNode(3);
    auto five = LiteralNode(5);
    auto coef_node_left = MultiplicationNode(&three, &var_node);
    auto coef_node_right = MultiplicationNode(&var_node, &five);
    auto sum_node_right = SumNode(&coef_node_right, &five);
    auto ct_node = GreaterThanOrEqualNode(&coef_node_left, &sum_node_right);
    Constraint constraint = {"ct_1", generateExpression(&ct_node)};
    std::vector<Constraint> vec_cts;
    vec_cts.push_back(std::move(constraint));

    auto model = model_builder.withId("model")
                   .withVariables(std::move(variables))
                   .withConstraints(std::move(vec_cts))
                   .build();

    auto component = component_builder.withId("componentTata")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();

    auto filler = std::make_unique<ComponentFiller>(component);
    auto pb = buildProblem({filler.get()});

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    auto ct = pb->getConstraint("componentTata.ct_1");
    BOOST_CHECK(ct);
    BOOST_CHECK_EQUAL(ct->getLb(), 5);
    BOOST_CHECK_EQUAL(ct->getUb(), pb->infinity());
    BOOST_CHECK_EQUAL(ct->getCoefficient(pb->getVariable("componentTata.var1")), -2);
}

// TODO
// - test ct with 2 vars
// - test component with two constraints

BOOST_AUTO_TEST_SUITE_END()
