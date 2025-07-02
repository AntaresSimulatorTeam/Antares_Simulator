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

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/study/system-model/component.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;

using namespace Antares::Optimization;

BOOST_AUTO_TEST_SUITE(_read_linear_expression_visitor_)

struct CreateVisitorFixture: Registry<Node>
{
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData data;
    EvaluationContext evaluationContext{{}, {}, data};
    SystemModel::Model m;
    SystemModel::ComponentBuilder componentBuilder;
    const SystemModel::Component component = componentBuilder.withId("compo")
                                               .withModel(&m)
                                               .withScenarioGroupId("group")
                                               .build();
    ReadLinearExpressionVisitor visitor{evaluationContext, {0, 0}, component};
};

BOOST_FIXTURE_TEST_CASE(name, CreateVisitorFixture)
{
    BOOST_CHECK_EQUAL(visitor.name(), "ReadLinearExpressionVisitor");
}

BOOST_FIXTURE_TEST_CASE(visit_literal, CreateVisitorFixture)
{
    Node* node = create<LiteralNode>(5.);
    auto linear_expression = visitor.dispatch(node).GetLinearExpressions().at(0);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 5.);
    BOOST_CHECK(linear_expression.coefPerVar().empty());
}

std::pair<std::string, ParameterTypeAndValue> build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const ParameterType& type = ParameterType::CONSTANT)
{
    return {id, {.id = id, .type = type, .value = value}};
}

BOOST_FIXTURE_TEST_CASE(visit_literal_plus_param, CreateVisitorFixture)
{
    // 5 + param(3) = 8
    Node* sum = create<SumNode>(create<LiteralNode>(5.), create<ParameterNode>("param"));
    EvaluationContext evaluation_context({{build_context_parameter_with("param", "3.")}}, {}, data);
    ReadLinearExpressionVisitor visitor(evaluation_context, {0, 0}, component);
    auto linear_expression = visitor.dispatch(sum).GetLinearExpressions().at(0);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 8.);
    BOOST_CHECK(linear_expression.coefPerVar().empty());
}

BOOST_FIXTURE_TEST_CASE(visit_literal_plus_param_plus_var, CreateVisitorFixture)
{
    // 60 + param(-5) + 7 * var = { 55, {var : 7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var"));
    Node* sum = create<SumNode>(create<LiteralNode>(60.), create<ParameterNode>("param"), product);
    EvaluationContext evaluation_context({{build_context_parameter_with("param", "-5.")}},
                                         {},
                                         data);
    ReadLinearExpressionVisitor visitor(evaluation_context, {0, 0}, component);
    auto linear_expression = visitor.dispatch(sum).GetLinearExpressions().at(0);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 55.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().at(FullKey(component.Id(), "var", 0, 0)), 7.);
}

struct MockLinearProblemData: Antares::Optimisation::LinearProblemApi::ILinearProblemData
{
    double getData(const std::string& dataSetId,
                   const std::string& scenarioGroup,
                   unsigned scenario,
                   unsigned hour) override
    {
        return hour; // for test
    }
};

BOOST_FIXTURE_TEST_CASE(visit_timeSum, CreateVisitorFixture)
{ // param  ={0,1,2}
    // 5 + sum( t-2 .. t+1, param) -->
    // t = 0 : 5 +param.at(-2) + param.at(-1) + param.at(0) + param.at(1) --> 5 +1 + 2 + 0 + 1 = 9
    // t = 1 : 5 +param.at(-1) + param.at(0) + param.at(1) + param.at(2) --> 5 + 2+0+1+2 = 10
    Node* from = create<LiteralNode>(-2.);
    Node* to = create<LiteralNode>(1.);
    Node* sum = create<SumNode>(create<LiteralNode>(5.),
                                create<TimeSumNode>(from, to, create<ParameterNode>("param")));
    MockLinearProblemData my_data;
    EvaluationContext evaluation_context(
      {{build_context_parameter_with("param", "something", ParameterType::TIMESERIE)}},
      {},
      my_data);
    ReadLinearExpressionVisitor visitor(evaluation_context, {0, 2}, component);
    auto linear_expressions = visitor.dispatch(sum).GetLinearExpressions();
    BOOST_CHECK_EQUAL(linear_expressions.at(0).offset(), 9.);
    BOOST_CHECK(linear_expressions.at(0).coefPerVar().empty());
    BOOST_CHECK_EQUAL(linear_expressions.at(1).offset(), 10.);
    BOOST_CHECK(linear_expressions.at(1).coefPerVar().empty());
}

BOOST_FIXTURE_TEST_CASE(visit_AllTimeSum, CreateVisitorFixture)
{ // param  ={0,1,2}
    // 5 + sum(param) -->
    // 5 +param.at(0) + param.at(1) + param.at(2)  --> 5 + 0 + 1  + 2  = 8

    Node* sum = create<SumNode>(create<LiteralNode>(5.),
                                create<AllTimeSumNode>(create<ParameterNode>("param")));
    MockLinearProblemData my_data;
    EvaluationContext evaluation_context(
      {{build_context_parameter_with("param", "something", ParameterType::TIMESERIE)}},
      {},
      my_data);
    ReadLinearExpressionVisitor visitor(evaluation_context, {0, 2}, component);
    auto linear_expressions = visitor.dispatch(sum).GetLinearExpressions();
    BOOST_CHECK_EQUAL(linear_expressions.at(0).offset(), 8.);
    BOOST_CHECK(linear_expressions.at(0).coefPerVar().empty());
}

BOOST_FIXTURE_TEST_CASE(visit_literal_plus_time_dependent_param_plus_var, CreateVisitorFixture)
{
    // 60 + param_at_0 + 7 * var = { 60, {var : 7} }
    // 60 + param_at_1 + 7 * var = { 61, {var : 7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var"));
    Node* sum = create<SumNode>(create<LiteralNode>(60.), create<ParameterNode>("param"), product);
    MockLinearProblemData my_data;
    EvaluationContext evaluation_context(
      {{build_context_parameter_with("param", "something", ParameterType::TIMESERIE)}},
      {},
      my_data);

    unsigned hour_0 = 0;
    unsigned hour_1 = 1;
    ReadLinearExpressionVisitor visitor(evaluation_context, {hour_0, hour_1}, component);
    auto linear_expressions = visitor.dispatch(sum).GetLinearExpressions();
    BOOST_CHECK_EQUAL(linear_expressions.at(0).offset(), 60.);
    BOOST_CHECK_EQUAL(linear_expressions.at(1).offset(), 61.);
    BOOST_CHECK_EQUAL(linear_expressions.at(0).coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linear_expressions.at(0).coefPerVar().at(
                        FullKey(component.Id(), "var", 0, 0)),
                      7.);
    BOOST_CHECK_EQUAL(linear_expressions.at(1).coefPerVar().at(
                        FullKey(component.Id(), "var", 0, 1)),
                      7.);
}

BOOST_FIXTURE_TEST_CASE(visit_param_declared_const_in_library_but_time_dep_in_system,
                        CreateVisitorFixture)
{
    ParameterNode p("param", TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
    EvaluationContext evaluation_context(
      {{build_context_parameter_with("param", "something", ParameterType::TIMESERIE)}},
      {},
      data);

    ReadLinearExpressionVisitor visitor(evaluation_context, {0, 1}, component);
    BOOST_CHECK_THROW(visitor.dispatch(&p), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(visit_negate_literal_plus_var, CreateVisitorFixture)
{
    // -(60 + 7 * var) = { -60, {var : -7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var"));
    Node* sum = create<SumNode>(create<LiteralNode>(60.), product);
    Node* neg = create<NegationNode>(sum);
    auto linear_expression = visitor.dispatch(neg).GetLinearExpressions().at(0);
    BOOST_CHECK_EQUAL(linear_expression.offset(), -60.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().at(FullKey(component.Id(), "var", 0, 0)), -7.);
}

BOOST_FIXTURE_TEST_CASE(visit_literal_minus_var, CreateVisitorFixture)
{
    // 60 - 7 * var = { 60, {var : -7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var"));
    Node* sub = create<SubtractionNode>(create<LiteralNode>(60.), product);
    auto linear_expression = visitor.dispatch(sub).GetLinearExpressions().at(0);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 60.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().at(FullKey(component.Id(), "var", 0, 0)), -7.);
}

BOOST_FIXTURE_TEST_CASE(visit_complex_expression, CreateVisitorFixture)
{
    // 2 * (13 + 3 * param1(-2) + 14 * var1) / 7 + param2(8) + 6 * var2 = {10 ; {var1:4, var2:6}}

    // small_sum = 13 + 3 * param1(-2) + 14 * var1
    Node* small_sum = create<SumNode>(create<LiteralNode>(13.),
                                      create<MultiplicationNode>(create<LiteralNode>(3),
                                                                 create<ParameterNode>("param1")),
                                      create<MultiplicationNode>(create<LiteralNode>(14),
                                                                 create<VariableNode>("var1")));

    // big_sum = 2 * small_sum / 7 + param2(8) + 6 * var2
    Node* big_sum = create<SumNode>(
      create<DivisionNode>(create<MultiplicationNode>(create<LiteralNode>(2.), small_sum),
                           create<LiteralNode>(7.)), // 2 * small_sum / 7
      create<ParameterNode>("param2"),               // param2
      create<MultiplicationNode>(create<LiteralNode>(6.), create<VariableNode>("var2")) // 6 * var2
    );

    EvaluationContext evaluation_context({build_context_parameter_with("param1", "-2."),
                                          build_context_parameter_with("param2", "8.")},
                                         {},
                                         data);
    ReadLinearExpressionVisitor visitor(evaluation_context, {0, 0}, component);
    auto linear_expression = visitor.dispatch(big_sum).GetLinearExpressions().at(0);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 10.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().at(FullKey(component.Id(), "var1", 0, 0)), 4.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().at(FullKey(component.Id(), "var2", 0, 0)), 6.);
}

BOOST_FIXTURE_TEST_CASE(comparison_nodes__exception_thrown, CreateVisitorFixture)
{
    Node* literal = create<LiteralNode>(5.);
    auto predicate = checkMessage("A linear expression can't contain comparison operators.");

    Node* node = create<EqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node), std::invalid_argument, predicate);

    node = create<LessThanOrEqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node), std::invalid_argument, predicate);

    node = create<GreaterThanOrEqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node), std::invalid_argument, predicate);
}

BOOST_FIXTURE_TEST_CASE(not_implemented_nodes__exception_thrown, CreateVisitorFixture)
{
    Node* node = create<PortFieldNode>("port", "field");
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node),
                          std::invalid_argument,
                          checkMessage("ReadLinearExpressionVisitor cannot visit PortFieldNodes"));

    node = create<ComponentVariableNode>("id", "y");
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node),
                          std::invalid_argument,
                          checkMessage(
                            "ReadLinearExpressionVisitor cannot visit ComponentVariableNodes"));

    node = create<ComponentParameterNode>("id", "y");
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node),
                          std::invalid_argument,
                          checkMessage(
                            "ReadLinearExpressionVisitor cannot visit ComponentParameterNodes"));
}

BOOST_AUTO_TEST_SUITE_END()
