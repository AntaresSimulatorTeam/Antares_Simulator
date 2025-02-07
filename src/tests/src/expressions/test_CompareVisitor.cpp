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

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/CloneVisitor.h>
#include <antares/expressions/visitors/CompareVisitor.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

struct ComparisonFixture
{
    Node* createSimpleExpression(double param);
    Node* createComplexExpression();

    Registry<Node> registry_;
};

Node* ComparisonFixture::createSimpleExpression(double param)
{
    Node* var1 = registry_.create<LiteralNode>(param);
    Node* param1 = registry_.create<ParameterNode>("param1");
    Node* expr = registry_.create<SumNode>(var1, param1);
    return expr;
}

Node* ComparisonFixture::createComplexExpression()
{
    // NOTE : this expression makes no sense, only for testing purposes
    // NOTE2 : Some elements are re-used (e.g simple), this is valid since memory is handled
    // separately (no double free)

    Node* simple = createSimpleExpression(42.);
    Node* neg = registry_.create<NegationNode>(simple);
    Node* mult = registry_.create<MultiplicationNode>(simple, neg);
    Node* comp = registry_.create<ComponentParameterNode>("hello", "world");
    Node* div = registry_.create<DivisionNode>(mult, comp);
    Node* div2 = registry_.create<DivisionNode>(div, simple);
    Node* add = registry_.create<SumNode>(div, div2, neg);
    Node* sub = registry_.create<SubtractionNode>(add, neg);
    Node* cmp = registry_.create<GreaterThanOrEqualNode>(sub, add);
    Node* pf = registry_.create<PortFieldNode>("port", "field");
    Node* pfsum = registry_.create<PortFieldSumNode>("port", "sum");
    Node* addf = registry_.create<SumNode>(pf, pfsum, cmp);
    return addf;
}

BOOST_AUTO_TEST_SUITE(_CompareVisitor_)

BOOST_FIXTURE_TEST_CASE(simple_comparison_to_itself, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr = createSimpleExpression(65.);
    BOOST_CHECK(cmp.dispatch(expr, expr));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_same, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr1 = createSimpleExpression(65.);
    Node* expr2 = createSimpleExpression(65.);
    BOOST_CHECK(cmp.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_different, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr1 = createSimpleExpression(64.);
    Node* expr2 = createSimpleExpression(65.);
    BOOST_CHECK(!cmp.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_self_complex, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr = createComplexExpression();
    BOOST_CHECK(cmp.dispatch(expr, expr));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_clone_complex, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr = createComplexExpression();
    CloneVisitor cloneVisitor(registry_);
    Node* expr_cloned = cloneVisitor.dispatch(expr);
    BOOST_CHECK(cmp.dispatch(expr, expr_cloned));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_complex, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr1 = createComplexExpression();
    Node* expr2 = createComplexExpression();
    BOOST_CHECK(cmp.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_different_complex, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr1 = createComplexExpression();
    Node* expr2 = registry_.create<NegationNode>(expr1);
    BOOST_CHECK(!cmp.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(CompareVisitor_name, Registry<Node>)
{
    CompareVisitor compareVisitor;
    BOOST_CHECK_EQUAL(compareVisitor.name(), "CompareVisitor");
}

BOOST_FIXTURE_TEST_CASE(compare_empty_sums, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* expr1 = registry_.create<SumNode>();
    Node* expr2 = registry_.create<SumNode>();
    BOOST_CHECK(compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_different_size_sums, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(2.);
    Node* expr1 = registry_.create<SumNode>(literal1, literal2);
    Node* expr2 = registry_.create<SumNode>(literal1);
    BOOST_CHECK(!compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_different_sums, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(2.);
    Node* literal3 = registry_.create<LiteralNode>(3.);
    Node* expr1 = registry_.create<SumNode>(literal1, literal2);
    Node* expr2 = registry_.create<SumNode>(literal1, literal3);
    BOOST_CHECK(!compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_same_sums, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(2.);
    Node* literal3 = registry_.create<LiteralNode>(3.);
    Node* expr1 = registry_.create<SumNode>(literal1, literal2, literal3);
    Node* expr2 = registry_.create<SumNode>(literal1, literal2, literal3);
    BOOST_CHECK(compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_same_sums_different_nodes, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(2.);
    Node* literal3 = registry_.create<LiteralNode>(2.);
    Node* expr1 = registry_.create<SumNode>(literal1, literal2);
    Node* expr2 = registry_.create<SumNode>(literal1, literal3);
    BOOST_CHECK(compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_sum_to_literal, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* expr1 = registry_.create<SumNode>(literal1);
    BOOST_CHECK(!compareVisitor.dispatch(expr1, literal1));
}
BOOST_AUTO_TEST_SUITE_END()
