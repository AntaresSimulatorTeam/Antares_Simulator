// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/optim-model-filler/LinearExpression.h>
#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>

using namespace Antares::Optimization;

BOOST_AUTO_TEST_SUITE(_linear_expressions_)

BOOST_AUTO_TEST_CASE(default_linear_expression)
{
    LinearExpression linearExpression;

    BOOST_CHECK_EQUAL(linearExpression.constant(), 0.);
    BOOST_CHECK_EQUAL(linearExpression.size(), 0);
}

BOOST_AUTO_TEST_CASE(linear_expression_explicit_construction)
{
    LinearExpression linearExpression({{0, -5.}}, 4.);

    BOOST_CHECK_EQUAL(linearExpression.constant(), 4.);
    BOOST_CHECK_EQUAL(linearExpression.size(), 1);
    BOOST_CHECK_EQUAL(linearExpression[0].first, 0);
    BOOST_CHECK_EQUAL(linearExpression[0].second, -5.);
}

BOOST_AUTO_TEST_CASE(sum_two_linear_expressions)
{
    LinearExpression linearExpression1({{0, -5.}, {1, 6.}}, 4.);
    LinearExpression linearExpression2({{1, -4.}, {2, 20.}}, -1.);

    auto sum = linearExpression1;
    sum += linearExpression2;
    sum.mergeDuplicateCoefficients();
    BOOST_CHECK_EQUAL(sum.constant(), 3.);
    BOOST_REQUIRE_EQUAL(sum.size(), 3);
    BOOST_CHECK_EQUAL(sum[0].second, -5.);
    BOOST_CHECK_EQUAL(sum[1].second, 2.);
    BOOST_CHECK_EQUAL(sum[2].second, 20.);
}

BOOST_AUTO_TEST_CASE(subtract_two_linear_expressions)
{
    LinearExpression linearExpression1({{0, -5.}, {1, 6.}}, 4.);
    LinearExpression linearExpression2({{1, -4.}, {2, 20.}}, -1.);

    auto sub = linearExpression1;
    sub -= linearExpression2;
    sub.mergeDuplicateCoefficients();
    BOOST_CHECK_EQUAL(sub.constant(), 5.);
    BOOST_REQUIRE_EQUAL(sub.size(), 3);
    BOOST_CHECK_EQUAL(sub[0].second, -5.);
    BOOST_CHECK_EQUAL(sub[1].second, 10.);
    BOOST_CHECK_EQUAL(sub[2].second, -20.);
}

BOOST_AUTO_TEST_CASE(multiply_linear_expression_by_scalar)
{
    LinearExpression linearExpression({{0, -5.}, {1, 6.}}, 4.);
    LinearExpression someScalar({}, -2.);

    auto product = linearExpression;
    product *= someScalar;

    BOOST_CHECK_EQUAL(product.constant(), -8.);
    BOOST_REQUIRE_EQUAL(product.size(), 2);
    BOOST_CHECK_EQUAL(product[0].second, 10.);
    BOOST_CHECK_EQUAL(product[1].second, -12.);
}

BOOST_AUTO_TEST_CASE(multiply_two_linear_expressions_containing_variables__exception_raised)
{
    LinearExpression linearExpression1({{0, -5.}, {1, 6.}}, 4.);
    LinearExpression linearExpression2({{1, -4.}, {2, 20.}}, -1.);

    BOOST_CHECK_EXCEPTION(linearExpression1 *= linearExpression2,
                          std::invalid_argument,
                          checkMessage("A linear expression can't have quadratic terms."));
}

BOOST_AUTO_TEST_CASE(divide_linear_expression_by_scalar)
{
    LinearExpression linearExpression({{0, -5.}, {1, 6.}}, 4.);
    LinearExpression someScalar({}, -2.);

    auto product = linearExpression / someScalar;

    BOOST_CHECK_EQUAL(product.constant(), -2.);
    BOOST_CHECK_EQUAL(product.size(), 2);
    BOOST_CHECK_EQUAL(product[0].first, 0);
    BOOST_CHECK_EQUAL(product[0].second, 2.5);
    BOOST_CHECK_EQUAL(product[1].first, 1);
    BOOST_CHECK_EQUAL(product[1].second, -3.);
}

BOOST_AUTO_TEST_CASE(divide_scalar_by_linear_expression__exception_raised)
{
    LinearExpression linearExpression({{0, -5.}, {1, 6.}}, 4.);
    LinearExpression someScalar({}, -2.);

    BOOST_CHECK_EXCEPTION(someScalar / linearExpression,
                          std::invalid_argument,
                          checkMessage("A linear expression can't have a variable as a dividend."));
}

BOOST_AUTO_TEST_CASE(negate_linear_expression)
{
    LinearExpression linearExpression({{0, -5.}, {1, 6.}}, 4.);

    auto negative = -linearExpression;

    BOOST_CHECK_EQUAL(negative.constant(), -4.);
    BOOST_CHECK_EQUAL(negative.size(), 2);
    BOOST_CHECK_EQUAL(negative[0].first, 0);
    BOOST_CHECK_EQUAL(negative[0].second, 5.);
    BOOST_CHECK_EQUAL(negative[1].first, 1);
    BOOST_CHECK_EQUAL(negative[1].second, -6.);
}

// Test default constructor
BOOST_AUTO_TEST_CASE(DefaultConstructor)
{
    TimeDependentLinearExpression expr(3);
    BOOST_CHECK_EQUAL(expr.size(), 3); // Should create expressions for 3 timesteps
}

BOOST_AUTO_TEST_CASE(sum_2_constant_expr)
{
    LinearExpression a(2.);
    LinearExpression b(4.);
    LinearExpression ret;
    ret += a;
    ret += b;
    BOOST_CHECK_EQUAL(ret.constant(), 2. + 4.);
}

BOOST_AUTO_TEST_CASE(sub_2_constant_expr)
{
    LinearExpression a(2.);
    LinearExpression b(4.);
    LinearExpression ret;
    ret += a;
    ret -= b;
    BOOST_CHECK_EQUAL(ret.constant(), 2. - 4.);
}

BOOST_AUTO_TEST_CASE(div_by_constant_expr)
{
    LinearExpression a(2.);
    LinearExpression b(4.);
    LinearExpression ret;
    ret += a;
    ret = ret / b;
    BOOST_CHECK_EQUAL(ret.constant(), 2. / 4.);
}

BOOST_AUTO_TEST_CASE(linear_expr_merge_dupl_coefficients_results_to_single_coeff)
{
    LinearExpression a({{0, 1.}, {0, 1.}}, 0.);
    a.mergeDuplicateCoefficients();
    BOOST_REQUIRE_EQUAL(a.size(), 1);
    const std::pair<int, double> expected{0, 2.};
    BOOST_CHECK(a[0] == expected);
}

BOOST_AUTO_TEST_CASE(linear_expr_merge_dupl_coefficients_results_to_two_coeffs)
{
    LinearExpression a({{0, 1.}, {1, 2.}}, 0.);
    a.mergeDuplicateCoefficients();
    BOOST_REQUIRE_EQUAL(a.size(), 2);
    const std::pair<int, double> expected1{0, 1.};
    BOOST_CHECK(a[0] == expected1);
    const std::pair<int, double> expected2{1, 2.};
    BOOST_CHECK(a[1] == expected2);
}

BOOST_AUTO_TEST_CASE(time_dep_duplicate_merge)
{
    std::vector<double> ct{1., 2., 3.};
    TimeDependentLinearExpression a(ct);
    a.mergeDuplicateCoefficients();
    BOOST_REQUIRE_EQUAL(a.size(), 3);
    BOOST_REQUIRE_EQUAL(a[0].size(), 0);
}

BOOST_AUTO_TEST_CASE(time_dep_add_size_known)
{
    std::vector<double> ct{1., 2., 3.};
    TimeDependentLinearExpression a(ct);
    TimeDependentLinearExpression b(ct);
    TimeDependentLinearExpression ret(3);
    ret += a;
    ret += b;
    BOOST_REQUIRE_EQUAL(ret.size(), 3);
    BOOST_REQUIRE_EQUAL(ret[0].constant(), 1. + 1.);
}

BOOST_AUTO_TEST_CASE(time_dep_iterate_operators)
{
    std::vector<double> ct{1., 2., 3.};
    const TimeDependentLinearExpression a(ct);
    for (std::size_t idx = 0; idx < ct.size(); idx++)
    {
        BOOST_CHECK_EQUAL(a[idx].size(), 0);
        BOOST_CHECK_EQUAL(a[idx].constant(), ct[idx]);
    }

    std::size_t idx = 0;
    for (const auto& expr: a)
    {
        BOOST_CHECK_EQUAL(expr.size(), 0);
        BOOST_CHECK_EQUAL(expr.constant(), ct[idx]);
        idx++;
    }
}

BOOST_AUTO_TEST_CASE(time_dep_rotate_twice)
{
    std::vector<double> ct{1., 2., 3.};
    TimeDependentLinearExpression a(ct);
    a.rotate(1);
    for (std::size_t idx = 0; idx < ct.size(); idx++)
    {
        BOOST_CHECK_EQUAL(a[idx].size(), 0);
        BOOST_CHECK_EQUAL(a[idx].constant(), ct[(idx + 1) % 3]);
    }
    a.rotate(1);
    for (std::size_t idx = 0; idx < ct.size(); idx++)
    {
        BOOST_CHECK_EQUAL(a[idx].size(), 0);
        BOOST_CHECK_EQUAL(a[idx].constant(), ct[(idx + 2) % 3]);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(OperatorCaretTestSuite)

BOOST_AUTO_TEST_CASE(ExponentZeroWithCoefficients)
{
    LinearExpression nonConstantExpr({{1, 2.0}, {2, 1.0}}, 3.0);
    LinearExpression exponent(0.0); // exposant = 0

    LinearExpression original = nonConstantExpr;

    nonConstantExpr ^= exponent;

    BOOST_CHECK(!nonConstantExpr.hasCoefs());
    BOOST_CHECK_EQUAL(nonConstantExpr.constant(), 1);
}

BOOST_AUTO_TEST_CASE(ExponentOneWithCoefficients)
{
    LinearExpression nonConstantExpr({{1, 2.0}, {2, 1.0}}, 3.0);
    LinearExpression exponent(1.0);
    LinearExpression original = nonConstantExpr;

    nonConstantExpr ^= exponent;

    // Avec exposant = 1, l'expression ne devrait pas changer structurellement
    BOOST_CHECK(nonConstantExpr.hasCoefs());
    BOOST_CHECK_EQUAL(nonConstantExpr.constant(), original.constant());
    BOOST_CHECK_EQUAL(original.size(), nonConstantExpr.size());
    for (auto i = 0; i < original.size(); ++i)
    {
        BOOST_CHECK_EQUAL(original[i].first, nonConstantExpr[i].first);
        BOOST_CHECK_EQUAL(original[i].second, nonConstantExpr[i].second);
    }
}

BOOST_AUTO_TEST_CASE(NonConstantExponentThrows)
{
    LinearExpression constantExpr(4.0);
    LinearExpression nonConstantExponent({{1, 1.0}}, 0.0); // exposant avec coefficients

    BOOST_CHECK_THROW(constantExpr ^= nonConstantExponent, std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ConstantExpressionWithNonUnitExponent)
{
    LinearExpression constantExpr(3.0);
    LinearExpression exponent(2.0); // exposant = 2
    constantExpr ^= exponent;
    BOOST_CHECK_EQUAL(constantExpr.constant(), 3 * 3 /*3^2*/);
}

BOOST_AUTO_TEST_CASE(ConstantExpressionWithExponentOne)
{
    LinearExpression constantExpr(5.0);
    LinearExpression exponent(1.0);

    constantExpr ^= exponent;

    // 5^1 = 5, ne devrait pas changer
    BOOST_CHECK_EQUAL(constantExpr.constant(), 5.0);
    BOOST_CHECK(!constantExpr.hasCoefs());
}

BOOST_AUTO_TEST_CASE(ZeroCoefficientsWithExponentZero)
{
    LinearExpression zeroCoefExpr({{1, 0.0}, {2, 0.0}}, 5.0);
    LinearExpression exponent(0.0);

    zeroCoefExpr ^= exponent;

    BOOST_CHECK(!zeroCoefExpr.hasCoefs());
}

BOOST_AUTO_TEST_CASE(ConstantExpressionWithExponentZero)
{
    LinearExpression constantExpr(7.0);
    LinearExpression exponent(0.0);

    constantExpr ^= exponent;

    // 7^0 = 1
    BOOST_CHECK_EQUAL(constantExpr.constant(), 1.0);
    BOOST_CHECK(!constantExpr.hasCoefs());
}

BOOST_AUTO_TEST_CASE(ZeroConstantWithExponentZero)
{
    LinearExpression zeroConstant(0.0);
    LinearExpression exponent(0.0);

    zeroConstant ^= exponent;

    // 0^0 = 1 (convention mathématique utilisée dans l'implémentation)
    BOOST_CHECK_EQUAL(zeroConstant.constant(), 1.0);
}

BOOST_AUTO_TEST_CASE(DecimalExponentThrowsForNonConstant)
{
    LinearExpression nonConstantExpr({{1, 1.0}}, 2.0);
    LinearExpression exponent(1.5); // exposant décimal

    BOOST_CHECK_THROW(nonConstantExpr ^= exponent, std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(NonLinearExponentThrows)
{
    LinearExpression nonConstantExpr({{1, 2.0}, {2, 1.0}}, 3.0);
    LinearExpression exponent(2.0); // exposant différent de 1

    // Doit lancer une exception car cela créerait des termes non linéaires
    BOOST_CHECK_THROW(nonConstantExpr ^= exponent, std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ExpressionWithOnlyCoefficientsExponentZero)
{
    LinearExpression onlyCoefficients({{1, 1.0}, {2, 2.0}}, 0.0);
    LinearExpression exponent(0.0);

    onlyCoefficients ^= exponent;

    BOOST_CHECK(!onlyCoefficients.hasCoefs());
    BOOST_CHECK(onlyCoefficients.constant() == 1);
}

BOOST_AUTO_TEST_CASE(ExpressionWithOnlyCoefficientsExponentOne)
{
    LinearExpression onlyCoefficients({{1, 1.0}, {2, 2.0}}, 0.0);
    LinearExpression exponent(1.0);

    onlyCoefficients ^= exponent;
    // Ne devrait pas changer avec exposant = 1
    BOOST_CHECK(onlyCoefficients.hasCoefs());
    BOOST_CHECK_EQUAL(onlyCoefficients.constant(), 0.0);
}

BOOST_AUTO_TEST_CASE(MixedZeroAndNonZeroCoefficientsExponentZero)
{
    LinearExpression mixedExpr({{1, 0.0}, {2, 1.0}}, 2.0);
    LinearExpression exponent(0.0);

    mixedExpr ^= exponent;

    BOOST_CHECK(!mixedExpr.hasCoefs());
}

BOOST_AUTO_TEST_SUITE_END()
