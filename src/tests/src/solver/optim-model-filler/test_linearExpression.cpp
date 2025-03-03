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
    LinearExpression linearExpression(4., {{"some key", -5.}});

    BOOST_CHECK_EQUAL(linearExpression.offset(), 4.);
    BOOST_CHECK_EQUAL(linearExpression.coefPerVar().size(), 1);
    BOOST_CHECK_EQUAL(linearExpression.coefPerVar()["some key"], -5.);
}

BOOST_AUTO_TEST_CASE(sum_two_linear_expressions)
{
    LinearExpression linearExpression1(4., {{"var1", -5.}, {"var2", 6.}});
    LinearExpression linearExpression2(-1., {{"var3", 20.}, {"var2", -4.}});

    auto sum = linearExpression1 + linearExpression2;

    BOOST_CHECK_EQUAL(sum.offset(), 3.);
    BOOST_CHECK_EQUAL(sum.coefPerVar().size(), 3);
    BOOST_CHECK_EQUAL(sum.coefPerVar()["var1"], -5.);
    BOOST_CHECK_EQUAL(sum.coefPerVar()["var2"], 2.);
    BOOST_CHECK_EQUAL(sum.coefPerVar()["var3"], 20.);
}

BOOST_AUTO_TEST_CASE(subtract_two_linear_expressions)
{
    LinearExpression linearExpression1(4., {{"var1", -5.}, {"var2", 6.}});
    LinearExpression linearExpression2(-1., {{"var2", -4.}, {"var3", 20.}});

    auto subtract = linearExpression1 - linearExpression2;

    BOOST_CHECK_EQUAL(subtract.offset(), 5.);
    BOOST_CHECK_EQUAL(subtract.coefPerVar().size(), 3);
    BOOST_CHECK_EQUAL(subtract.coefPerVar()["var1"], -5.);
    BOOST_CHECK_EQUAL(subtract.coefPerVar()["var2"], 10.);
    BOOST_CHECK_EQUAL(subtract.coefPerVar()["var3"], -20.);
}

BOOST_AUTO_TEST_CASE(multiply_linear_expression_by_scalar)
{
    LinearExpression linearExpression(4., {{"var1", -5.}, {"var2", 6.}});
    LinearExpression someScalar(-2., {});

    auto product = linearExpression * someScalar;

    BOOST_CHECK_EQUAL(product.offset(), -8.);
    BOOST_CHECK_EQUAL(product.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(product.coefPerVar()["var1"], 10.);
    BOOST_CHECK_EQUAL(product.coefPerVar()["var2"], -12.);
}

BOOST_AUTO_TEST_CASE(multiply_scalar_by_linear_expression)
{
    LinearExpression linearExpression(4., {{"var1", -5.}, {"var2", 6.}});
    LinearExpression someScalar(-2., {});

    auto product = someScalar * linearExpression;

    BOOST_CHECK_EQUAL(product.offset(), -8.);
    BOOST_CHECK_EQUAL(product.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(product.coefPerVar()["var1"], 10.);
    BOOST_CHECK_EQUAL(product.coefPerVar()["var2"], -12.);
}

BOOST_AUTO_TEST_CASE(multiply_two_linear_expressions_containing_variables__exception_raised)
{
    LinearExpression linearExpression1(4., {{"var1", -5.}, {"var2", 6.}});
    LinearExpression linearExpression2(-1., {{"var2", -4.}, {"var3", 20.}});

    BOOST_CHECK_EXCEPTION(linearExpression1 * linearExpression2,
                          std::invalid_argument,
                          checkMessage("A linear expression can't have quadratic terms."));
}

BOOST_AUTO_TEST_CASE(divide_linear_expression_by_scalar)
{
    LinearExpression linearExpression(4., {{"var1", -5.}, {"var2", 6.}});
    LinearExpression someScalar(-2., {});

    auto product = linearExpression / someScalar;

    BOOST_CHECK_EQUAL(product.offset(), -2.);
    BOOST_CHECK_EQUAL(product.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(product.coefPerVar()["var1"], 2.5);
    BOOST_CHECK_EQUAL(product.coefPerVar()["var2"], -3.);
}

BOOST_AUTO_TEST_CASE(divide_scalar_by_linear_expression__exception_raised)
{
    LinearExpression linearExpression(4., {{"var1", -5.}, {"var2", 6.}});
    LinearExpression someScalar(-2., {});

    BOOST_CHECK_EXCEPTION(someScalar / linearExpression,
                          std::invalid_argument,
                          checkMessage("A linear expression can't have a variable as a dividend."));
}

BOOST_AUTO_TEST_CASE(negate_linear_expression)
{
    LinearExpression linearExpression(4., {{"var1", -5.}, {"var2", 6.}});

    auto negative = -linearExpression;

    BOOST_CHECK_EQUAL(negative.offset(), -4.);
    BOOST_CHECK_EQUAL(negative.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(negative.coefPerVar()["var1"], 5.);
    BOOST_CHECK_EQUAL(negative.coefPerVar()["var2"], -6.);
}

// Test default constructor
BOOST_AUTO_TEST_CASE(DefaultConstructor)
{
    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2);
    TimeDependentLinearExpression expr(context);
    BOOST_TEST(expr.getSize() == 3); // Should create expressions for 3 timesteps
}

// Test constructor with a single LinearExpression
BOOST_AUTO_TEST_CASE(ConstructorWithLinearExpression)
{
    Antares::Optimisation::LinearProblemApi::FillContext context(0, 2);
    LinearExpression le(5.0, {{"x", 2.0}});
    TimeDependentLinearExpression expr(context, le);

    auto expressions = expr.GetLinearExpressions();
    BOOST_TEST(expressions.size() == 3);
    for (const auto& [timestep, lexpr]: expressions)
    {
        BOOST_TEST(lexpr.offset() == 5.0);
        BOOST_TEST(lexpr.coefPerVar().at("x") == 2.0);
    }
}

// Test constructor with a map of LinearExpression objects
BOOST_AUTO_TEST_CASE(ConstructorWithMap)
{
    std::map<unsigned, LinearExpression> expressions = {{0, LinearExpression(1.0, {{"a", 1.5}})},
                                                        {1, LinearExpression(2.0, {{"b", 3.0}})}};

    TimeDependentLinearExpression expr(expressions);
    BOOST_TEST(expr.getSize() == 2);
    BOOST_TEST(expr.GetLinearExpressions().at(0).offset() == 1.0);
    BOOST_TEST(expr.GetLinearExpressions().at(1).offset() == 2.0);
}

// Test addition operator
BOOST_AUTO_TEST_CASE(AdditionOperator)
{
    std::map<unsigned, LinearExpression> exp1 = {{0, LinearExpression(3.0, {{"x", 1.0}})},
                                                 {1, LinearExpression(2.0, {{"y", 2.0}})}};
    std::map<unsigned, LinearExpression> exp2 = {{0, LinearExpression(2.0, {{"x", 2.0}})},
                                                 {1, LinearExpression(1.0, {{"y", 1.0}})}};

    TimeDependentLinearExpression expr1(exp1), expr2(exp2);
    TimeDependentLinearExpression result = expr1 + expr2;

    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 5.0);
    BOOST_TEST(result.GetLinearExpressions().at(0).coefPerVar().at("x") == 3.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).coefPerVar().at("y") == 3.0);
}

// Test subtraction operator
BOOST_AUTO_TEST_CASE(SubtractionOperator)
{
    std::map<unsigned, LinearExpression> exp1 = {{0, LinearExpression(5.0, {{"x", 4.0}})},
                                                 {1, LinearExpression(7.0, {{"y", 3.0}})}};
    std::map<unsigned, LinearExpression> exp2 = {{0, LinearExpression(3.0, {{"x", 2.0}})},
                                                 {1, LinearExpression(2.0, {{"y", 1.0}})}};

    TimeDependentLinearExpression expr1(exp1), expr2(exp2);
    TimeDependentLinearExpression result = expr1 - expr2;

    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 2.0);
    BOOST_TEST(result.GetLinearExpressions().at(0).coefPerVar().at("x") == 2.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).coefPerVar().at("y") == 2.0);
}

// Test multiplication operator
BOOST_AUTO_TEST_CASE(MultiplicationOperator)
{
    std::map<unsigned, LinearExpression> exp1 = {{0, LinearExpression(2.0, {{"x", 3.0}})}};
    std::map<unsigned, LinearExpression> exp2 = {
      {0, LinearExpression(4.0, {})} // Only scalar allowed
    };

    TimeDependentLinearExpression expr1(exp1), expr2(exp2);
    TimeDependentLinearExpression result = expr1 * expr2;

    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 8.0);
    BOOST_TEST(result.GetLinearExpressions().at(0).coefPerVar().at("x") == 12.0);
}

// Test division operator
BOOST_AUTO_TEST_CASE(DivisionOperator)
{
    std::map<unsigned, LinearExpression> exp1 = {{0, LinearExpression(6.0, {{"x", 3.0}})}};
    std::map<unsigned, LinearExpression> exp2 = {
      {0, LinearExpression(2.0, {})} // Only scalar allowed
    };

    TimeDependentLinearExpression expr1(exp1), expr2(exp2);
    TimeDependentLinearExpression result = expr1 / expr2;

    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == 3.0);
    BOOST_TEST(result.GetLinearExpressions().at(0).coefPerVar().at("x") == 1.5);
}

// Test negation
BOOST_AUTO_TEST_CASE(NegationOperator)
{
    std::map<unsigned, LinearExpression> exp = {{0, LinearExpression(3.0, {{"x", 2.0}})},
                                                {1, LinearExpression(4.0, {{"y", 1.0}})}};

    TimeDependentLinearExpression expr(exp);
    TimeDependentLinearExpression result = -expr;

    BOOST_TEST(result.GetLinearExpressions().at(0).offset() == -3.0);
    BOOST_TEST(result.GetLinearExpressions().at(0).coefPerVar().at("x") == -2.0);
    BOOST_TEST(result.GetLinearExpressions().at(1).coefPerVar().at("y") == -1.0);
}

// Test GetLinearExpressions
BOOST_AUTO_TEST_CASE(GetLinearExpressionsMethod)
{
    std::map<unsigned, LinearExpression> exp = {{0, LinearExpression(5.0, {{"x", 2.0}})},
                                                {1, LinearExpression(3.0, {{"y", 4.0}})}};

    TimeDependentLinearExpression expr(exp);
    auto expressions = expr.GetLinearExpressions();

    BOOST_TEST(expressions.size() == 2);
    BOOST_TEST(expressions.at(0).offset() == 5.0);
    BOOST_TEST(expressions.at(1).coefPerVar().at("y") == 4.0);
}

// Test getSize()
BOOST_AUTO_TEST_CASE(GetSizeMethod)
{
    std::map<unsigned, LinearExpression> exp = {{0, LinearExpression(1.0, {{"x", 1.0}})},
                                                {1, LinearExpression(2.0, {{"y", 2.0}})},
                                                {2, LinearExpression(3.0, {{"z", 3.0}})}};

    TimeDependentLinearExpression expr(exp);
    BOOST_TEST(expr.getSize() == 3);
}

BOOST_AUTO_TEST_SUITE_END()
