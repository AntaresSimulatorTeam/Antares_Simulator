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

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/CloneVisitor.h>
#include <antares/solver/expressions/visitors/CompareVisitor.h>
#include <antares/solver/expressions/visitors/PrintVisitor.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;
using namespace Antares::Solver::Visitors;

// Only necessary for BOOST_CHECK_EQUAL

BOOST_AUTO_TEST_SUITE(_CompareVisitor_)

static Node* createSimpleExpression(Registry<Node>& registry, double param)
{
    Node* var1 = registry.create<LiteralNode>(param);
    Node* param1 = registry.create<ParameterNode>("param1");
    Node* expr = registry.create<AddNode>(var1, param1);
    return expr;
}

BOOST_FIXTURE_TEST_CASE(comparison_to_self_simple, Registry<Node>)
{
    CompareVisitor cmp;
    Node* expr = createSimpleExpression(*this, 65.);
    BOOST_CHECK(cmp.dispatch(*expr, *expr));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_same, Registry<Node>)
{
    CompareVisitor cmp;
    auto create = [this] { return createSimpleExpression(*this, 65.); };
    Node* expr1 = create();
    Node* expr2 = create();
    BOOST_CHECK(cmp.dispatch(*expr1, *expr2));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_different, Registry<Node>)
{
    CompareVisitor cmp;
    Node* expr1 = createSimpleExpression(*this, 64.);
    Node* expr2 = createSimpleExpression(*this, 65.);
    BOOST_CHECK(!cmp.dispatch(*expr1, *expr2));
}

static Node* createComplexExpression(Registry<Node>& registry)
{
    // NOTE : this expression makes no sense, only for testing purposes
    // NOTE2 : Some elements are re-used (e.g simple), this is valid since memory is handled
    // separately (no double free)

    Node* simple = createSimpleExpression(registry, 42.);
    Node* neg = registry.create<NegationNode>(simple);
    Node* mult = registry.create<MultiplicationNode>(simple, neg);
    Node* comp = registry.create<ComponentParameterNode>("hello", "world");
    Node* div = registry.create<DivisionNode>(mult, comp);
    Node* div2 = registry.create<DivisionNode>(div, simple);
    Node* add = registry.create<AddNode>(div, div2);
    Node* sub = registry.create<SubtractionNode>(add, neg);
    Node* cmp = registry.create<GreaterThanOrEqualNode>(sub, add);
    Node* pf = registry.create<PortFieldNode>("port", "field");
    Node* addf = registry.create<AddNode>(pf, cmp);
    return addf;
}

BOOST_FIXTURE_TEST_CASE(comparison_to_self_complex, Registry<Node>)
{
    CompareVisitor cmp;
    Node* expr = createComplexExpression(*this);
    BOOST_CHECK(cmp.dispatch(*expr, *expr));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_clone_complex, Registry<Node>)
{
    CompareVisitor cmp;
    Node* expr = createComplexExpression(*this);
    CloneVisitor cloneVisitor(*this);
    Node* expr_cloned = cloneVisitor.dispatch(*expr);
    BOOST_CHECK(cmp.dispatch(*expr, *expr_cloned));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_complex, Registry<Node>)
{
    CompareVisitor cmp;
    Node* expr1 = createComplexExpression(*this);
    Node* expr2 = createComplexExpression(*this);
    BOOST_CHECK(cmp.dispatch(*expr1, *expr2));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_different_complex, Registry<Node>)
{
    CompareVisitor cmp;
    Node* expr1 = createComplexExpression(*this);
    Node* expr2 = create<NegationNode>(expr1);
    BOOST_CHECK(!cmp.dispatch(*expr1, *expr2));
}
BOOST_AUTO_TEST_SUITE_END()
