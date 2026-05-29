// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

BOOST_FIXTURE_TEST_CASE(visit_max_two_literals, VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* five = create<LiteralNode>(5.);
    Node* six = create<LiteralNode>(6.);
    Node* max = create<FunctionNode>(FunctionNodeType::max, five, six);
    auto linear_expression = visitor().dispatch(max);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 6.);
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_min_two_literals, VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* five = create<LiteralNode>(5.);
    Node* six = create<LiteralNode>(6.);
    Node* min = create<FunctionNode>(FunctionNodeType::min, five, six);
    auto linear_expression = visitor().dispatch(min);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 5.);
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_pow_two_literals, VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* five = create<LiteralNode>(5.);
    Node* six = create<LiteralNode>(6.);
    Node* pow = create<FunctionNode>(FunctionNodeType::pow, five, six);
    auto linear_expression = visitor().dispatch(pow);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), std::pow(5, 6));
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_pow_simple_linear_0, VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* variable = create<VariableNode>("variable", 0 /* index */);
    Node* eight = create<LiteralNode>(8);
    Node* zero = create<LiteralNode>(0.);
    Node* mult = create<MultiplicationNode>(eight, variable);
    Node* pow = create<FunctionNode>(FunctionNodeType::pow, mult, zero); // (8 * variable)^0
    auto linear_expression = visitor().dispatch(pow);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_pow_simple_linear_1, VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* variable = create<VariableNode>("variable", 0);
    Node* eight = create<LiteralNode>(8);
    Node* one = create<LiteralNode>(1.);
    Node* mult = create<MultiplicationNode>(eight, variable);
    Node* pow = create<FunctionNode>(FunctionNodeType::pow, mult, one); // (8 * variable)^1
    auto linear_expression = visitor().dispatch(pow);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 0);
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 1);
    BOOST_CHECK(linear_expression[0].hasCoefs());
    BOOST_CHECK_EQUAL(linear_expression[0].begin()->second, 8);
}

BOOST_FIXTURE_TEST_CASE(visit_pow_simple__non_linear_linear,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* variable = create<VariableNode>("variable", 0);
    Node* eight = create<LiteralNode>(8);
    Node* six = create<LiteralNode>(6.);
    Node* mult = create<MultiplicationNode>(eight, variable);
    Node* pow = create<FunctionNode>(FunctionNodeType::pow, mult, six); // (8 * variable)^6
    BOOST_CHECK_THROW(visitor().dispatch(pow), std::invalid_argument);
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

BOOST_FIXTURE_TEST_CASE(visit__max_literal__param, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // max(5, param(3)) = 5
    Node* maxNode = create<FunctionNode>(FunctionNodeType::max,
                                         create<LiteralNode>(5.),
                                         create<ParameterNode>("param_3"));
    auto linear_expression = visitor().dispatch(maxNode);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 5.);
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit__min_literal__param, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // min(5, param(3)) = 3
    Node* minNode = create<FunctionNode>(FunctionNodeType::min,
                                         create<LiteralNode>(5.),
                                         create<ParameterNode>("param_3"));
    auto linear_expression = visitor().dispatch(minNode);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 3.);
    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(linear_expr_from_floor_of_a_literal,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // floor(4.5) = 4
    Node* floorNode = create<FunctionNode>(FunctionNodeType::floor, create<LiteralNode>(4.5));

    auto linear_expression = visitor().dispatch(floorNode);

    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 4.);
    BOOST_CHECK_EQUAL(linear_expression[0].hasCoefs(), false); // Not variable
}

BOOST_FIXTURE_TEST_CASE(linear_expr_from_floor_of_a_constant_param,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // floor(param(4.5)) = 4
    Node* floorNode = create<FunctionNode>(FunctionNodeType::floor,
                                           create<ParameterNode>("four.five"));

    auto linear_expression = visitor().dispatch(floorNode);

    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 4.);
    BOOST_CHECK_EQUAL(linear_expression[0].hasCoefs(), false); // Not variable
}

BOOST_FIXTURE_TEST_CASE(linear_expr_from_floor_of_a_variable_throws,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // floor(7 * var) is not linear: argument has non-zero coefficients
    Node* var = create<VariableNode>("var", 0);
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.), var);
    Node* floorNode = create<FunctionNode>(FunctionNodeType::floor, product);

    BOOST_CHECK_EXCEPTION(visitor().dispatch(floorNode),
                          std::invalid_argument,
                          checkMessage(
                            "floor operator: its argument is not constant, but has to be."));
}

BOOST_FIXTURE_TEST_CASE(linear_expr_from_ceil_of_a_literal,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // ceil(4.5) = 5
    Node* ceilNode = create<FunctionNode>(FunctionNodeType::ceil, create<LiteralNode>(4.5));

    auto linear_expression = visitor().dispatch(ceilNode);

    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 5.);
    BOOST_CHECK_EQUAL(linear_expression[0].hasCoefs(), false); // Not variable
}

BOOST_FIXTURE_TEST_CASE(linear_expr_from_ceil_of_a_constant_param,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // ceil(param(4.5)) = 5
    Node* ceilNode = create<FunctionNode>(FunctionNodeType::ceil,
                                          create<ParameterNode>("four.five"));

    auto linear_expression = visitor().dispatch(ceilNode);

    BOOST_REQUIRE_EQUAL(linear_expression.size(), 1);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 5.);
    BOOST_CHECK_EQUAL(linear_expression[0].hasCoefs(), false); // Not variable
}

BOOST_FIXTURE_TEST_CASE(linear_expr_from_ceil_of_a_variable_throws,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // ceil(7 * var) is not linear: argument has non-zero coefficients
    Node* var = create<VariableNode>("var", 0);
    Node* product = create<MultiplicationNode>(create<LiteralNode>(7.), var);
    Node* ceilNode = create<FunctionNode>(FunctionNodeType::ceil, product);

    BOOST_CHECK_EXCEPTION(visitor().dispatch(ceilNode),
                          std::invalid_argument,
                          checkMessage(
                            "ceil operator: its argument is not constant, but has to be."));
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
    // param = {0, 1, 2}
    // 5 + sum( t-2 .. t+1, param) :
    //
    // t = 0 :
    // 5 + param[-2] + param[-1] + param[0] + param[1] = 5 + (1 + 2 + 0 + 1) = 9
    //
    // t = 1 :
    // 5 +param[-1] + param[0] + param[1] + param[2] --> 5 + (2 + 0 + 1 + 2) = 10
    Node* from = create<TPlusNode>(create<LiteralNode>(-2.));
    Node* to = create<TPlusNode>(create<LiteralNode>(1.));
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

BOOST_FIXTURE_TEST_CASE(visit_timeSum_with_mixed_bounds,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // param = {0, 1, 2}
    // 5 + sum( 1 .. t+1, param)
    Node* from = create<LiteralNode>(1.);
    Node* to = create<TPlusNode>(create<LiteralNode>(1.));
    Node* sum = create<SumNode>(create<LiteralNode>(5.),
                                create<TimeSumNode>(from, to, create<ParameterNode>("param_ts")));

    ctx = LinearProblemApi::FillContext(0, 2, 0, 2, 0);

    auto linear_expression = visitor().dispatch(sum);
    BOOST_REQUIRE_EQUAL(linear_expression.size(), 3);

    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 6.);
    BOOST_CHECK_EQUAL(linear_expression[1].constant(), 8.);
    BOOST_CHECK_EQUAL(linear_expression[2].constant(), 8.);

    BOOST_CHECK_EQUAL(linear_expression[0].size(), 0);
    BOOST_CHECK_EQUAL(linear_expression[1].size(), 0);
    BOOST_CHECK_EQUAL(linear_expression[2].size(), 0);
}

BOOST_FIXTURE_TEST_CASE(visit_timeSum_with_time_dependent_fixed_bound_throws,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* from = create<VariableNode>("var1", 0, VariabilityType::VARYING_IN_TIME_ONLY);
    Node* to = create<LiteralNode>(2.);
    Node* sum = create<TimeSumNode>(from, to, create<LiteralNode>(1.));

    BOOST_CHECK_EXCEPTION(visitor().dispatch(sum),
                          Antares::Error::InvalidArgumentError,
                          checkMessage("A sum bound must be fixed in time in 'var1'."));
}

BOOST_FIXTURE_TEST_CASE(visit_timeSum_with_time_dependent_moving_bound_throws,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    Node* from = create<LiteralNode>(1.);
    Node* to = create<TPlusNode>(create<VariableNode>("var1", 0));
    Node* sum = create<TimeSumNode>(from, to, create<LiteralNode>(1.));

    BOOST_CHECK_EXCEPTION(visitor().dispatch(sum),
                          Antares::Error::InvalidArgumentError,
                          checkMessage("A sum bound must be fixed in time in 't+var1'."));
}

BOOST_FIXTURE_TEST_CASE(visit_AllTimeSum, VisitorFixture<ReadLinearExpressionVisitor>)
{
    // param = {0, 1, 2}
    // 5 + sum(param) = 5 + param[0] + param[1] + param[2] = 5 + (0 + 1 + 2)  = 8

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

BOOST_FIXTURE_TEST_CASE(visit_timeSum_with_time_dependent_variable_coefficients,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // var1 is time-dependent, so the number of coefficients in the sum reflects the window size.
    // 0..3, 1..3, 2..3, 3..3 -> 4, 3, 2, 1 coefficients.
    Node* var = create<VariableNode>("var1", 0, VariabilityType::VARYING_IN_TIME_ONLY);

    ctx = LinearProblemApi::FillContext(0, 3, 0, 3, 0);

    auto sum = create<TimeSumNode>(create<LiteralNode>(1.),
                                   create<TPlusNode>(create<LiteralNode>(0)),
                                   var);

    auto expr = visitor().dispatch(sum);

    BOOST_REQUIRE_EQUAL(expr[0].size(), 0); // sum(1..0, var1) = 0
    BOOST_REQUIRE_EQUAL(expr[1].size(), 1); // sum(1..1, var1) = var1[1]
    BOOST_REQUIRE_EQUAL(expr[2].size(), 2); // sum(1..2, var1) = var1[1] + var1[2]
    BOOST_REQUIRE_EQUAL(expr[3].size(), 3); // sum(1..3, var1) = var1[1] + var1[2] + var1[3]
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

    node = create<FunctionNode>(FunctionNodeType::dual,
                                create<ParameterNode>("constraint1"),
                                create<LiteralNode>(0));
    BOOST_CHECK_EXCEPTION(visitor().dispatch(node),
                          Antares::Error::InvalidArgumentError,
                          checkMessage(
                            "A linear expression can't contain extra output operator dual."));

    node = create<FunctionNode>(FunctionNodeType::reduced_cost, create<VariableNode>("var", 0));
    BOOST_CHECK_EXCEPTION(
      visitor().dispatch(node),
      Antares::Error::InvalidArgumentError,
      checkMessage("A linear expression can't contain extra output operator reduced_cost."));
}

BOOST_FIXTURE_TEST_CASE(visit_timeSum_with_inverted_bounds_returns_zero,
                        VisitorFixture<ReadLinearExpressionVisitor>)
{
    // sum(t+5 .. t+1, param) with from > to should return 0
    Node* from = create<TPlusNode>(create<LiteralNode>(5.));
    Node* to = create<TPlusNode>(create<LiteralNode>(1.));
    Node* sum = create<TimeSumNode>(from, to, create<ParameterNode>("param_ts"));

    ctx = LinearProblemApi::FillContext(0, 2, 0, 2, 0);
    auto linear_expression = visitor().dispatch(sum);

    BOOST_REQUIRE_EQUAL(linear_expression.size(), 3);
    BOOST_CHECK_EQUAL(linear_expression[0].constant(), 0.);
    BOOST_CHECK_EQUAL(linear_expression[1].constant(), 0.);
    BOOST_CHECK_EQUAL(linear_expression[2].constant(), 0.);
}

BOOST_AUTO_TEST_SUITE_END()
