/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

#include "visitorFixture.hpp"
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;
using namespace Antares::Optimization;

BOOST_AUTO_TEST_SUITE(_read_linear_expression_visitor_)

BOOST_FIXTURE_TEST_CASE(name, VisitorFixture<ReadLinearExpressionVisitor>)
{
    BOOST_CHECK_EQUAL(visitor().name(), "ReadLinearExpressionVisitor");
}

BOOST_FIXTURE_TEST_CASE(visit_literal, VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* node = create<LiteralNode>(5.);
    auto linear_expression = visitor().dispatch(node);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 5.);
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_literal_plus_param, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // 5 + param(3) = 8
    Node* sum = create<SumNode>(create<LiteralNode>(5.), create<ParameterNode>("param_3"));
    auto linear_expression = visitor().dispatch(sum);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 8.);
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_literal_plus_param_plus_var,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // 60 + param(-5) + 7 * var = { 55, {var : 7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var", 0));
    Node* sum = create<SumNode>(create<LiteralNode>(60.),
                                create<ParameterNode>("param_m5"),
                                product);
    auto linear_expression = visitor().dispatch(sum);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1 /* timestep (constant expression) */);

    // Constant
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 55.);

    // Coefs
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0][0].first, 0);
    BOOST_CHECK_EQUAL(linear_expression[0][0].second, 7.);
}

BOOST_FIXTURE_TEST_CASE(visit_timeSum, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // param = {0,1,2}
    // 5 + sum( t-2 .. t+1, param) -->
    // t = 0 : 5 +param.at(-2) + param.at(-1) + param.at(0) + param.at(1) --> 5 +1 + 2 + 0 + 1 = 9
    // t = 1 : 5 +param.at(-1) + param.at(0) + param.at(1) + param.at(2) --> 5 + 2+0+1+2 = 10
    Node* from = create<LiteralNode>(-2.);
    Node* to = create<LiteralNode>(1.);
    Node* sum = create<SumNode>(create<LiteralNode>(5.),
                                create<TimeSumNode>(from, to, create<ParameterNode>("param_ts")));

    ctx = LinearProblemApi::FillContext(0, 2, 0, 2, 0);

    auto linear_expression = visitor().dispatch(sum);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 3 /* timesteps */);

    // Constants
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 9.);
    BOOST_CHECK_EQUAL(linear_expression[1].constant(), 10.);

    // Coefs
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
    BOOST_CHECK_EQUAL(linear_expression[1].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_AllTimeSum, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // param = {0,1,2}
    // 5 + sum(param) -->
    // 5 +param.at(0) + param.at(1) + param.at(2)  --> 5 + 0 + 1  + 2  = 8

    Node* sum = create<SumNode>(create<LiteralNode>(5.),
                                create<AllTimeSumNode>(create<ParameterNode>("param_ts")));

    ctx = LinearProblemApi::FillContext(0, 2, 0, 2, 0);
    auto linear_expression = visitor().dispatch(sum);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 3 /* timesteps */);

    // Constants
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 8.);

    // Coefs
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_literal_plus_time_dependent_param_plus_var,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // 60 + param_at_0 + 7 * var = { 60, {var : 7} }
    // 60 + param_at_1 + 7 * var = { 61, {var : 7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var", 0));
    Node* sum = create<SumNode>(create<LiteralNode>(60.),
                                create<ParameterNode>("param_ts"),
                                product);

    unsigned hour_0 = 0;
    unsigned hour_1 = 1;
    ctx = LinearProblemApi::FillContext(hour_0, hour_1, hour_0, hour_1, 0);
    auto linear_expression = visitor().dispatch(sum);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 2 /* timesteps */);
    // Constants
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 60.);
    BOOST_CHECK_EQUAL(linear_expression[1].constant(), 61.);

    // Coefs
    auto checkAt = [&](unsigned int t)
    {
        BOOST_REQUIRE_EQUAL(linear_expression[t].size(), 1);
        BOOST_CHECK_EQUAL(linear_expression[t][0].first, t);
        BOOST_CHECK_EQUAL(linear_expression[t][0].second, 7.);
    };
    checkAt(0);
    checkAt(1);
}

BOOST_FIXTURE_TEST_CASE(visit_param_declared_const_in_library_but_time_dep_in_system,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    ParameterNode p("param_ts", Antares::Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_THROW(visitor().dispatch(&p), Antares::Error::InvalidArgumentError);
}

BOOST_FIXTURE_TEST_CASE(visit_negate_literal_plus_var, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // -(60 + 7 * var) = { -60, {var : -7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var", 0));
    Node* sum = create<SumNode>(create<LiteralNode>(60.), product);
    Node* neg = create<NegationNode>(sum);
    auto linear_expression = visitor().dispatch(neg);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1 /* timesteps */);

    // Constants
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), -60.);

    // Coefs
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0][0].first, 0);
    BOOST_CHECK_EQUAL(linear_expression[0][0].second, -7.);
}

BOOST_FIXTURE_TEST_CASE(visit_literal_minus_var, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // 60 - 7 * var = { 60, {var : -7} }
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.),
                                               create<VariableNode>("var", 0));
    Node* sub = create<SubtractionNode>(create<LiteralNode>(60.), product);
    auto linear_expression = visitor().dispatch(sub);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1 /* timesteps */);

    // Constants
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 60.);

    // Coefs
    BOOST_REQUIRE_EQUAL(linear_expression[0].size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0][0].first, 0);
    BOOST_CHECK_EQUAL(linear_expression[0][0].second, -7.);
}

BOOST_FIXTURE_TEST_CASE(visit_complex_expression, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // 2 * (13 + 3 * param1(-2) + 14 * var1) / 7 + param2(8) + 6 * var2 = {10 ; {var1:4, var2:6}}

    // small_sum = 13 + 3 * param1(-2) + 14 * var1
    Node* small_sum = create<SumNode>(create<LiteralNode>(13.),
                                      create<MultiplicationNode>(create<LiteralNode>(3),
                                                                 create<ParameterNode>("param1")),
                                      create<MultiplicationNode>(create<LiteralNode>(14),
                                                                 create<VariableNode>("var1", 0)));

    // big_sum = 2 * small_sum / 7 + param2(8) + 6 * var2
    Node* big_sum = create<SumNode>(
      create<DivisionNode>(create<MultiplicationNode>(create<LiteralNode>(2.), small_sum),
                           create<LiteralNode>(7.)), // 2 * small_sum / 7
      create<ParameterNode>("param2"),               // param2
      create<MultiplicationNode>(create<LiteralNode>(6.),
                                 create<VariableNode>("var2", 1)) // 6 * var2
    );

    auto linear_expression = visitor().dispatch(big_sum);
    // Constants
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 10.);

    // Coefs
    BOOST_REQUIRE_EQUAL(linear_expression[0].size(), 2);

    BOOST_CHECK_EQUAL(linear_expression[0][0].first, 0);
    BOOST_CHECK_EQUAL(linear_expression[0][0].second, 4.);

    BOOST_CHECK_EQUAL(linear_expression[0][1].first, 1);
    BOOST_CHECK_EQUAL(linear_expression[0][1].second, 6.);
}

BOOST_FIXTURE_TEST_CASE(comparison_nodes__exception_thrown,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* literal = create<LiteralNode>(5.);
    auto predicate = checkMessage("A linear expression can't contain comparison operators.");

    Node* node = create<EqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor().dispatch(node),
                          Antares::Error::InvalidArgumentError,
                          predicate);

    node = create<LessThanOrEqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor().dispatch(node),
                          Antares::Error::InvalidArgumentError,
                          predicate);

    node = create<GreaterThanOrEqualNode>(literal, literal);
    BOOST_CHECK_EXCEPTION(visitor().dispatch(node),
                          Antares::Error::InvalidArgumentError,
                          predicate);
}

BOOST_FIXTURE_TEST_CASE(not_implemented_nodes__exception_thrown,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* node = create<PortFieldNode>("port", "field");
    BOOST_CHECK_EXCEPTION(visitor().dispatch(node),
                          Antares::Error::InvalidArgumentError,
                          checkMessage("ReadLinearExpressionVisitor cannot visit PortFieldNodes"));

    node = create<DualNode>("constraint1", 0);
    BOOST_CHECK_EXCEPTION(visitor().dispatch(node),
                          Antares::Error::InvalidArgumentError,
                          checkMessage(
                            "A linear expression can't contain extra output operator dual."));

    node = create<ReducedCostNode>("var", 0, TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EXCEPTION(
      visitor().dispatch(node),
      Antares::Error::InvalidArgumentError,
      checkMessage("A linear expression can't contain extra output operator reduced_cost."));
}

BOOST_AUTO_TEST_SUITE_END()
