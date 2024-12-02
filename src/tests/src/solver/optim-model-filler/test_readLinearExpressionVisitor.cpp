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

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;
using namespace Antares::Solver::Visitors;

using namespace Antares::Optimization;

BOOST_AUTO_TEST_SUITE(_read_linear_expression_visitor_)

BOOST_AUTO_TEST_CASE(name)
{
    ReadLinearExpressionVisitor visitor;
    BOOST_CHECK_EQUAL(visitor.name(), "ReadLinearExpressionVisitor");
}

BOOST_FIXTURE_TEST_CASE(visit_literal, Registry<Node>)
{
    Node* node = create<LiteralNode>(5.);
    ReadLinearExpressionVisitor visitor;
    auto linear_expression = visitor.dispatch(node);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 5.);
    BOOST_CHECK(linear_expression.coefPerVar().empty());
}

BOOST_FIXTURE_TEST_CASE(visit_literal_plus_param, Registry<Node>)
{
    // 5 + param(3) = 8
    Node* sum = create<SumNode>(create<LiteralNode>(5.), create<ParameterNode>("param"));
    EvaluationContext evaluation_context({{"param", 3.}}, {});
    ReadLinearExpressionVisitor visitor(evaluation_context);
    auto linear_expression = visitor.dispatch(sum);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 8.);
    BOOST_CHECK(linear_expression.coefPerVar().empty());
}

BOOST_FIXTURE_TEST_CASE(visit_literal_plus_param_plus_var, Registry<Node>)
{
    // 60 + param(-5) + 7 * var = { 55, {var : 7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var"));
    Node* sum = create<SumNode>(create<LiteralNode>(60.), create<ParameterNode>("param"), product);
    EvaluationContext evaluation_context({{"param", -5.}}, {});
    ReadLinearExpressionVisitor visitor(evaluation_context);
    auto linear_expression = visitor.dispatch(sum);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 55.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar()["var"], 7.);
}

BOOST_FIXTURE_TEST_CASE(visit_negate_literal_plus_var, Registry<Node>)
{
    // -(60 + 7 * var) = { -60, {var : -7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var"));
    Node* sum = create<SumNode>(create<LiteralNode>(60.), product);
    Node* neg = create<NegationNode>(sum);
    ReadLinearExpressionVisitor visitor;
    auto linear_expression = visitor.dispatch(neg);
    BOOST_CHECK_EQUAL(linear_expression.offset(), -60.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar()["var"], -7.);
}

BOOST_FIXTURE_TEST_CASE(visit_literal_minus_var, Registry<Node>)
{
    // 60 - 7 * var = { 60, {var : -7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var"));
    Node* sub = create<SubtractionNode>(create<LiteralNode>(60.), product);
    ReadLinearExpressionVisitor visitor;
    auto linear_expression = visitor.dispatch(sub);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 60.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar()["var"], -7.);
}

BOOST_FIXTURE_TEST_CASE(visit_complex_expression, Registry<Node>)
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

    EvaluationContext evaluation_context({{"param1", -2.}, {"param2", 8.}}, {});
    ReadLinearExpressionVisitor visitor(evaluation_context);
    auto linear_expression = visitor.dispatch(big_sum);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 10.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar()["var1"], 4.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar()["var2"], 6.);
}

BOOST_FIXTURE_TEST_CASE(comparison_nodes__exception_thrown, Registry<Node>)
{
    Node* literal = create<LiteralNode>(5.);
    ReadLinearExpressionVisitor visitor;
    auto predicate = checkMessage("A linear expression can't contain comparison operators.");

    Node* node = create<EqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node), std::invalid_argument, predicate);

    node = create<LessThanOrEqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node), std::invalid_argument, predicate);

    node = create<GreaterThanOrEqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node), std::invalid_argument, predicate);
}

BOOST_FIXTURE_TEST_CASE(not_implemented_nodes__exception_thrown, Registry<Node>)
{
    ReadLinearExpressionVisitor visitor;

    Node* node = create<PortFieldNode>("port", "field");
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node),
                          std::invalid_argument,
                          checkMessage("ReadLinearExpressionVisitor cannot visit PortFieldNodes"));

    node = create<PortFieldSumNode>("port", "field");
    BOOST_CHECK_EXCEPTION(visitor.dispatch(node),
                          std::invalid_argument,
                          checkMessage(
                            "ReadLinearExpressionVisitor cannot visit PortFieldSumNodes"));

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
