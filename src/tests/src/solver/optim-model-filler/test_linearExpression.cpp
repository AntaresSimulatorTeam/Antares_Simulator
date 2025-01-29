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

#include <antares/solver/optim-model-filler/LinearExpression.h>

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

    auto negative = linearExpression.negate();

    BOOST_CHECK_EQUAL(negative.offset(), -4.);
    BOOST_CHECK_EQUAL(negative.coefPerVar().size(), 2);
    BOOST_CHECK_EQUAL(negative.coefPerVar()["var1"], 5.);
    BOOST_CHECK_EQUAL(negative.coefPerVar()["var2"], -6.);
}

BOOST_AUTO_TEST_SUITE_END()
