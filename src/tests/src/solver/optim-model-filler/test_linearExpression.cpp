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
#if 0
#define WIN32_LEAN_AND_MEAN

#include <ranges>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>

using namespace Antares::Optimization;

BOOST_AUTO_TEST_SUITE(_linear_expressions_)

BOOST_AUTO_TEST_CASE(default_linear_expression)
{
    LinearExpression linearExpression;

    BOOST_CHECK_EQUAL(linearExpression.offset(), 0.);
    BOOST_CHECK(linearExpression.coefPerVar().empty());
}

BOOST_AUTO_TEST_CASE(linear_expression_explicit_construction)
{
    LinearExpression linearExpression(4., {{FullKey("compo", "var"), -5.}});

    BOOST_CHECK_EQUAL(linearExpression.offset(), 4.);
    BOOST_CHECK_EQUAL(linearExpression.coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linearExpression.coefPerVar().at(FullKey("compo", "var")), -5.);
}

BOOST_AUTO_TEST_CASE(sum_two_linear_expressions)
{
    auto component = "compo";
    LinearExpression linearExpression1(4.,
                                       {{FullKey(component, "var1"), -5.},
                                        {FullKey(component, "var2"), 6.}});
    LinearExpression linearExpression2(-1.,
                                       {{FullKey(component, "var3"), 20.},
                                        {FullKey(component, "var2"), -4.}});

    auto sum = linearExpression1 + linearExpression2;

    BOOST_CHECK_EQUAL(sum.offset(), 3.);
    BOOST_CHECK_EQUAL(sum.coefPerVar().size(), 3);
    BOOST_CHECK_EQUAL(sum.coefPerVar().at(FullKey(component, "var1")), -5.);
    BOOST_CHECK_EQUAL(sum.coefPerVar().at(FullKey(component, "var2")), 2.);
    BOOST_CHECK_EQUAL(sum.coefPerVar().at(FullKey(component, "var3")), 20.);
}

BOOST_AUTO_TEST_CASE(subtract_two_linear_expressions)
{
    auto component = "compo";

    LinearExpression linearExpression1(4.,
                                       {{FullKey(component, "var1"), -5.},
                                        {FullKey(component, "var2"), 6.}});
    LinearExpression linearExpression2(-1.,
                                       {{FullKey(component, "var2"), -4.},
                                        {FullKey(component, "var3"), 20.}});

    auto subtract = linearExpression1 - linearExpression2;

    BOOST_CHECK_EQUAL(subtract.offset(), 5.);
    BOOST_CHECK_EQUAL(subtract.coefPerVar().size(), 3);
    BOOST_CHECK_EQUAL(subtract.coefPerVar().at(FullKey(component, "var1")), -5.);
    BOOST_CHECK_EQUAL(subtract.coefPerVar().at(FullKey(component, "var2")), 10.);
    BOOST_CHECK_EQUAL(subtract.coefPerVar().at(FullKey(component, "var3")), -20.);
}

BOOST_AUTO_TEST_CASE(multiply_linear_expression_by_scalar)
{
    auto component = "compo";

    LinearExpression linearExpression(4.,
                                      {{FullKey(component, "var1"), -5.},
                                       {FullKey(component, "var2"), 6.}});
    LinearExpression someScalar(-2., {});

    auto product = linearExpression * someScalar;

    BOOST_CHECK_EQUAL(product.offset(), -8.);
    BOOST_CHECK_EQUAL(product.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(product.coefPerVar().at(FullKey(component, "var1")), 10.);
    BOOST_CHECK_EQUAL(product.coefPerVar().at(FullKey(component, "var2")), -12.);
}

BOOST_AUTO_TEST_CASE(multiply_scalar_by_linear_expression)
{
    auto component = "compo";

    LinearExpression linearExpression(4.,
                                      {{FullKey(component, "var1"), -5.},
                                       {FullKey(component, "var2"), 6.}});
    LinearExpression someScalar(-2., {});

    auto product = someScalar * linearExpression;

    BOOST_CHECK_EQUAL(product.offset(), -8.);
    BOOST_CHECK_EQUAL(product.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(product.coefPerVar().at(FullKey(component, "var1")), 10.);
    BOOST_CHECK_EQUAL(product.coefPerVar().at(FullKey(component, "var2")), -12.);
}

BOOST_AUTO_TEST_CASE(multiply_two_linear_expressions_containing_variables__exception_raised)
{
    auto component = "compo";

    LinearExpression linearExpression1(4.,
                                       {{FullKey(component, "var1"), -5.},
                                        {FullKey(component, "var2"), 6.}});
    LinearExpression linearExpression2(-1.,
                                       {{FullKey(component, "var2"), -4.},
                                        {FullKey(component, "var3"), 20.}});

    BOOST_CHECK_EXCEPTION(linearExpression1 * linearExpression2,
                          std::invalid_argument,
                          checkMessage("A linear expression can't have quadratic terms."));
}

BOOST_AUTO_TEST_CASE(divide_linear_expression_by_scalar)
{
    auto component = "compo";

    LinearExpression linearExpression(4.,
                                      {{FullKey(component, "var1"), -5.},
                                       {FullKey(component, "var2"), 6.}});
    LinearExpression someScalar(-2., {});

    auto product = linearExpression / someScalar;

    BOOST_CHECK_EQUAL(product.offset(), -2.);
    BOOST_CHECK_EQUAL(product.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(product.coefPerVar().at(FullKey(component, "var1")), 2.5);
    BOOST_CHECK_EQUAL(product.coefPerVar().at(FullKey(component, "var2")), -3.);
}

BOOST_AUTO_TEST_CASE(divide_scalar_by_linear_expression__exception_raised)
{
    auto component = "compo";

    LinearExpression linearExpression(4.,
                                      {{FullKey(component, "var1"), -5.},
                                       {FullKey(component, "var2"), 6.}});
    LinearExpression someScalar(-2., {});

    BOOST_CHECK_EXCEPTION(someScalar / linearExpression,
                          std::invalid_argument,
                          checkMessage("A linear expression can't have a variable as a dividend."));
}

BOOST_AUTO_TEST_CASE(negate_linear_expression)
{
    auto component = "compo";

    LinearExpression linearExpression(4.,
                                      {{FullKey(component, "var1"), -5.},
                                       {FullKey(component, "var2"), 6.}});

    auto negative = -linearExpression;

    BOOST_CHECK_EQUAL(negative.offset(), -4.);
    BOOST_CHECK_EQUAL(negative.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(negative.coefPerVar().at(FullKey(component, "var1")), 5.);
    BOOST_CHECK_EQUAL(negative.coefPerVar().at(FullKey(component, "var2")), -6.);
}

// Test default constructor
BOOST_AUTO_TEST_CASE(DefaultConstructor)
{
    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    TimeDependentLinearExpression expr(context);
    BOOST_TEST(expr.getSize() == 3); // Should create expressions for 3 timesteps
}

// Test constructor with a single LinearExpression
BOOST_AUTO_TEST_CASE(ConstructorWithLinearExpression)
{
    auto component = "compo";

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    LinearExpression le(5.0, {{FullKey(component, "x"), 2.0}});
    TimeDependentLinearExpression expr(context, le);

    auto expressions = expr.GetLinearExpressions();
    BOOST_TEST(expressions.size() == 3);
    for (const auto& [timestep, lexpr]: expressions)
    {
        BOOST_TEST(lexpr.offset() == 5.0);
        BOOST_TEST(lexpr.coefPerVar().at(FullKey(component, "x")) == 2.0);
    }
}

// Test constructor with a map of LinearExpression objects
BOOST_AUTO_TEST_CASE(ConstructorWithMap)
{
    auto component = "compo";

    LinearExpressionMap expressions = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.5}})},
                                       {1,
                                        LinearExpression(2.0, {{FullKey(component, "b"), 3.0}})}};

    TimeDependentLinearExpression expr({0, 2, 0, 2, 0}, expressions);
    BOOST_TEST(expr.getSize() == 2);
    BOOST_TEST(expr.GetLinearExpressions().at(0).offset() == 1.0);
    BOOST_TEST(expr.GetLinearExpressions().at(1).offset() == 2.0);
}

// Test addition operator
BOOST_AUTO_TEST_CASE(AdditionOperator)
{
    auto component = "compo";

    LinearExpressionMap exp1 = {{0, LinearExpression(3.0, {{FullKey(component, "x"), 1.0}})},
                                {1, LinearExpression(2.0, {{FullKey(component, "y"), 2.0}})}};
    LinearExpressionMap exp2 = {{0, LinearExpression(2.0, {{FullKey(component, "x"), 2.0}})},
                                {1, LinearExpression(1.0, {{FullKey(component, "y"), 1.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    TimeDependentLinearExpression expr1(context, exp1), expr2(context, exp2);
    expr1 += expr2;

    BOOST_TEST(expr1.GetLinearExpressions().at(0).offset() == 5.0);
    BOOST_TEST(expr1.GetLinearExpressions().at(0).coefPerVar().at(FullKey(component, "x")) == 3.0);
    BOOST_TEST(expr1.GetLinearExpressions().at(1).coefPerVar().at(FullKey(component, "y")) == 3.0);
}

// Test subtraction operator
BOOST_AUTO_TEST_CASE(SubtractionOperator)
{
    auto component = "compo";

    LinearExpressionMap exp1 = {{0, LinearExpression(5.0, {{FullKey(component, "x"), 4.0}})},
                                {1, LinearExpression(7.0, {{FullKey(component, "y"), 3.0}})}};
    LinearExpressionMap exp2 = {{0, LinearExpression(3.0, {{FullKey(component, "x"), 2.0}})},
                                {1, LinearExpression(2.0, {{FullKey(component, "y"), 1.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    TimeDependentLinearExpression expr1(context, exp1), expr2(context, exp2);
    expr1 -= expr2;

    BOOST_TEST(expr1.GetLinearExpressions().at(0).offset() == 2.0);
    BOOST_TEST(expr1.GetLinearExpressions().at(0).coefPerVar().at(FullKey(component, "x")) == 2.0);
    BOOST_TEST(expr1.GetLinearExpressions().at(1).coefPerVar().at(FullKey(component, "y")) == 2.0);
}

// Test multiplication operator
BOOST_AUTO_TEST_CASE(MultiplicationOperator)
{
    auto component = "compo";
    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);

    LinearExpressionMap exp1 = {{0, LinearExpression(2.0, {{FullKey(component, "x"), 3.0}})}};
    LinearExpressionMap exp2 = {
      {0, LinearExpression(4.0, {})} // Only scalar allowed
    };

    TimeDependentLinearExpression expr1(context, exp1), expr2(context, exp2);
    TimeDependentLinearExpression result = expr1 * expr2;

    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 8.0);
    BOOST_TEST(result.GetLinearExpressions().at(0).coefPerVar().at(FullKey(component, "x"))
               == 12.0);
}

// Test division operator
BOOST_AUTO_TEST_CASE(DivisionOperator)
{
    auto component = "compo";

    LinearExpressionMap exp1 = {{0, LinearExpression(6.0, {{FullKey(component, "x"), 3.0}})}};
    LinearExpressionMap exp2 = {
      {0, LinearExpression(2.0, {})} // Only scalar allowed
    };

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    TimeDependentLinearExpression expr1(context, exp1), expr2(context, exp2);
    TimeDependentLinearExpression result = expr1 / expr2;

    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 3.0);
    BOOST_TEST(result.GetLinearExpressions().at(0).coefPerVar().at(FullKey(component, "x")) == 1.5);
}

// Test negation
BOOST_AUTO_TEST_CASE(NegationOperator)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(3.0, {{FullKey(component, "x"), 2.0}})},
                               {1, LinearExpression(4.0, {{FullKey(component, "y"), 1.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    TimeDependentLinearExpression expr(context, exp);
    TimeDependentLinearExpression result = -expr;

    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == -3.0);
    BOOST_TEST(result.GetLinearExpressions().at(0).coefPerVar().at(FullKey(component, "x"))
               == -2.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).coefPerVar().at(FullKey(component, "y"))
               == -1.0);
}

// Test GetLinearExpressions
BOOST_AUTO_TEST_CASE(GetLinearExpressionsMethod)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(5.0, {{FullKey(component, "x"), 2.0}})},
                               {1, LinearExpression(3.0, {{FullKey(component, "y"), 4.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    TimeDependentLinearExpression expr(context, exp);
    auto expressions = expr.GetLinearExpressions();

    BOOST_TEST(expressions.size() == 2);
    BOOST_TEST(expressions.at(0).offset() == 5.0);
    BOOST_TEST(expressions.at(1).coefPerVar().at(FullKey(component, "y")) == 4.0);
}

// Test getSize()
BOOST_AUTO_TEST_CASE(GetSizeMethod)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "x"), 1.0}})},
                               {1, LinearExpression(2.0, {{FullKey(component, "y"), 2.0}})},
                               {2, LinearExpression(3.0, {{FullKey(component, "z"), 3.0}})}};
    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    TimeDependentLinearExpression expr(context, exp);
    BOOST_TEST(expr.getSize() == 3);
}

// Test shiftLinearExpressions with positive shift
BOOST_AUTO_TEST_CASE(ShiftLinearExpressionsPositive)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {2, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    Antares::Optimization::TimeDependentLinearExpression result = expr.shiftLinearExpressions(1);

    BOOST_TEST(result.getSize() == 3);
    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 2.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).offset() == 3.0);
    BOOST_TEST(result.GetLinearExpressions().at(2).offset() == 1.0);
}

// Test shiftLinearExpressions with negative shift
BOOST_AUTO_TEST_CASE(ShiftLinearExpressionsNegative)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {2, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};
    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    Antares::Optimization::TimeDependentLinearExpression result = expr.shiftLinearExpressions(-1);

    BOOST_TEST(result.getSize() == 3);
    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 3.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).offset() == 1.0);
    BOOST_TEST(result.GetLinearExpressions().at(2).offset() == 2.0);
}

// Test shiftLinearExpressions with zero shift
BOOST_AUTO_TEST_CASE(ShiftLinearExpressionsZero)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {2, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    Antares::Optimization::TimeDependentLinearExpression result = expr.shiftLinearExpressions(0);

    BOOST_TEST(result.getSize() == 3);
    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 1.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).offset() == 2.0);
    BOOST_TEST(result.GetLinearExpressions().at(2).offset() == 3.0);
}

// Test shiftLinearExpressions with shift greater than size
BOOST_AUTO_TEST_CASE(ShiftLinearExpressionsGreaterThanSize)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {2, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    Antares::Optimization::TimeDependentLinearExpression result = expr.shiftLinearExpressions(4);

    BOOST_TEST(result.getSize() == 3);
    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 2.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).offset() == 3.0);
    BOOST_TEST(result.GetLinearExpressions().at(2).offset() == 1.0);
}

// Test operator[] with valid index
BOOST_AUTO_TEST_CASE(OperatorIndexValid)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {2, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    Antares::Optimization::TimeDependentLinearExpression indexed = expr[1];

    const auto result = indexed.GetLinearExpressions().cbegin()->second;
    BOOST_TEST(result.offset() == 2.0);
    BOOST_TEST(result.coefPerVar().at(FullKey(component, "b")) == 2.0);
}

// Test operator[] with invalid index
BOOST_AUTO_TEST_CASE(OperatorIndexInvalid)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {2, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    BOOST_CHECK_THROW(expr[3], std::out_of_range);
}

// Test shiftLinearExpressions with negative shift greater than size
BOOST_AUTO_TEST_CASE(ShiftLinearExpressionsNegativeGreaterThanSize)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {2, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2, 0, 2, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    Antares::Optimization::TimeDependentLinearExpression result = expr.shiftLinearExpressions(-4);

    BOOST_TEST(result.getSize() == 3);
    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 3.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).offset() == 1.0);
    BOOST_TEST(result.GetLinearExpressions().at(2).offset() == 2.0);
}

// Test timeSumLinearExpressions
BOOST_AUTO_TEST_CASE(TimeSumLinearExpressions)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(1.0, {{FullKey(component, "a"), -18.0}})},
                               {2, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {3, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 3, 0, 3, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    Antares::Optimization::TimeDependentLinearExpression result = expr.timeSumLinearExpressions(0,
                                                                                                3);

    BOOST_CHECK_EQUAL(result.getSize(), 4);
    for (const auto& expression: result.GetLinearExpressions() | std::views::values)
    {
        BOOST_TEST(expression.offset() == (1.0 + 1.0 + 2.0 + 3.0));
        BOOST_TEST(expression.coefPerVar().at(FullKey(component, "a")) == (1.0 - 18.0));
        BOOST_TEST(expression.coefPerVar().at(FullKey(component, "b")) == 2.0);
        BOOST_TEST(expression.coefPerVar().at(FullKey(component, "c")) == 3.0);
    }
}

// Test allTimeSumLinearExpressions
BOOST_AUTO_TEST_CASE(AllTimeSumLinearExpressions)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(1.0, {{FullKey(component, "a"), -18.0}})},
                               {2, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {3, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 3, 0, 3, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);
    Antares::Optimization::TimeDependentLinearExpression result = expr
                                                                    .allTimeSumLinearExpressions();

    BOOST_TEST(result.getSize() == 4);
    for (const auto& expression: result.GetLinearExpressions() | std::views::values)
    {
        BOOST_TEST(expression.offset() == (1.0 + 1.0 + 2.0 + 3.0));
        BOOST_TEST(expression.coefPerVar().at(FullKey(component, "a")) == (1.0 - 18.0));
        BOOST_TEST(expression.coefPerVar().at(FullKey(component, "b")) == 2.0);
        BOOST_TEST(expression.coefPerVar().at(FullKey(component, "c")) == 3.0);
    }
}

// Test timeSumLinearExpressions with differents shift values
BOOST_AUTO_TEST_CASE(TimeSumLinearExpressionsOutOfBounds)
{
    auto component = "compo";

    LinearExpressionMap exp = {{0, LinearExpression(1.0, {{FullKey(component, "a"), 1.0}})},
                               {1, LinearExpression(1.0, {{FullKey(component, "a"), -18.0}})},
                               {2, LinearExpression(2.0, {{FullKey(component, "b"), 2.0}})},
                               {3, LinearExpression(3.0, {{FullKey(component, "c"), 3.0}})}};

    Antares::Optimisation::LinearProblemApi::FillContext context(0, 3, 0, 3, 0);
    Antares::Optimization::TimeDependentLinearExpression expr(context, exp);

    // Case where 'from' is negative
    Antares::Optimization::TimeDependentLinearExpression result1 = expr.timeSumLinearExpressions(-1,
                                                                                                 2);
    BOOST_CHECK_EQUAL(result1.getSize(), 4);
    for (const auto& expression: result1.GetLinearExpressions() | std::views::values)
    {
        BOOST_TEST(expression.offset() == (1.0 + 1.0 + 2.0 + 3.0));
    }

    // Case where 'to' is greater than the last time step
    Antares::Optimization::TimeDependentLinearExpression result2 = expr.timeSumLinearExpressions(0,
                                                                                                 5);
    BOOST_TEST(result2.getSize() == 4);
    const auto result2_linearExpressions = result2.GetLinearExpressions();
    BOOST_TEST(result2_linearExpressions.at(0).offset() == (1.0 + 1.0 + 2.0 + 3.0 + 1.0 + 1.0));
    BOOST_TEST(result2_linearExpressions.at(1).offset() == (1.0 + 2.0 + 3.0 + 1.0 + 1.0 + 2.0));
    BOOST_TEST(result2_linearExpressions.at(2).offset() == (2.0 + 3.0 + 1.0 + 1.0 + 2.0 + 3.0));
    BOOST_TEST(result2_linearExpressions.at(3).offset() == (3.0 + 1.0 + 1.0 + 2.0 + 3.0 + 1.0));

    // Case where 'from' > 'to'
    Antares::Optimization::TimeDependentLinearExpression result3 = expr.timeSumLinearExpressions(2,
                                                                                                 0);
    BOOST_TEST(result3.getSize() == 4);
    for (const auto& expression: result3.GetLinearExpressions() | std::views::values)
    {
        BOOST_TEST(expression.offset() == 0.);
        BOOST_TEST(expression.coefPerVar().empty());
    }
}

BOOST_AUTO_TEST_SUITE_END()
#endif
