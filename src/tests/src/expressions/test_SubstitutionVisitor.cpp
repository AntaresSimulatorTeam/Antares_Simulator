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
#include <antares/expressions/visitors/CompareVisitor.h>
#include <antares/expressions/visitors/PortFieldSubstitutionVisitor.h>
#include <antares/expressions/visitors/PortFieldSumSubstitutionVisitor.h>
#include <antares/expressions/visitors/PrintVisitor.h>
#include <antares/expressions/visitors/SubstitutionVisitor.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

// Only necessary for BOOST_CHECK_EQUAL

BOOST_AUTO_TEST_SUITE(_SubstitutionVisitor_)

std::pair<Nodes::ComponentVariableNode*, Nodes::ComponentVariableNode*> fillContext(
  SubstitutionContext& ctx,
  Registry<Node>& registry)
{
    auto add = [&ctx, &registry](const std::string& component, const std::string& variable)
    {
        auto* in = registry.create<ComponentVariableNode>(component, variable);
        ctx.variables.insert(in);
        return in;
    };
    return {add("component1", "variable1"), add("component2", "variable1")};
}

BOOST_FIXTURE_TEST_CASE(SubstitutionVisitor_substitute_one_node, Registry<Node>)
{
    SubstitutionContext ctx;
    auto variables = fillContext(ctx, *this);

    auto* component_original = create<ComponentVariableNode>("component1", "notInThere");

    Node* root = create<SumNode>(create<ComponentVariableNode>("component1", "variable1"),
                                 component_original);
    SubstitutionVisitor sub(*this, ctx);
    Node* subsd = sub.dispatch(root);

    // The root of the new tree should be different
    BOOST_CHECK_NE(root, subsd);

    // We expect to find a substituted node on the left
    BOOST_CHECK_EQUAL((*dynamic_cast<SumNode*>(subsd))[0], variables.first);

    // We expect to find an original node on the right
    auto* right_substituted = (*dynamic_cast<SumNode*>(subsd))[1];
    BOOST_CHECK_NE(right_substituted, variables.first);
    BOOST_CHECK_NE(right_substituted, variables.second);

    auto* component = dynamic_cast<ComponentVariableNode*>(right_substituted);
    BOOST_REQUIRE(component);
    // We don't use BOOST_CHECK_EQUAL because operator<<(..., const ComponentVariableNode&) is
    // not implemented
    BOOST_CHECK(*component == *component_original);
}

BOOST_FIXTURE_TEST_CASE(SubstitutionVisitor_name, Registry<Node>)
{
    SubstitutionContext ctx;

    SubstitutionVisitor substitutionVisitor(*this, ctx);
    BOOST_CHECK_EQUAL(substitutionVisitor.name(), "SubstitutionVisitor");
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_PortfieldSubstitutionVisitor_)

class SubstitutionFixture: public Registry<Node>
{
public:
    Node* originalExpression()
    {
        Node* port1 = create<PortFieldNode>("port", "literal");
        Node* port2 = create<PortFieldNode>("another port", "not a literal");
        Node* root = create<SumNode>(port1, port2);
        return root;
    }

    Node* expectedExpressionAfterSubstitution()
    {
        Node* node1 = create<LiteralNode>(10);
        Node* port2 = create<PortFieldNode>("another port", "not a literal");
        Node* root = create<SumNode>(node1, port2);
        return root;
    }

    Node* substitute(Node* original)
    {
        PortFieldSubstitutionContext ctx;
        ctx.portfield.emplace(std::piecewise_construct,
                              std::forward_as_tuple("port", "literal"),
                              std::forward_as_tuple(create<LiteralNode>(10)));

        PortFieldSubstitutionVisitor sub(*this, ctx);
        return sub.dispatch(original);
    }
};

BOOST_FIXTURE_TEST_CASE(PortfieldSubstitutionVisitor_simple, SubstitutionFixture)

{
    Node* original = originalExpression();
    Node* substituted = substitute(original);
    Node* expected = expectedExpressionAfterSubstitution();

    CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(substituted, expected));
}

BOOST_FIXTURE_TEST_CASE(PortfieldSubstitutionVisitor_name, Registry<Node>)
{
    PortFieldSubstitutionContext ctx;

    PortFieldSubstitutionVisitor substitutionVisitor(*this, ctx);
    BOOST_CHECK_EQUAL(substitutionVisitor.name(), "PortFieldSubstitutionVisitor");
}

class SumSubstitutionFixture: public Registry<Node>
{
public:
    Node* originalExpression()
    {
        Node* port1 = create<PortFieldSumNode>("port", "sum of literal");
        Node* port2 = create<PortFieldNode>("another port", "another field");
        Node* root = create<SumNode>(port1, port2);
        return root;
    }

    Node* expectedExpressionAfterSubstitution()
    {
        Node* node1 = create<SumNode>(create<LiteralNode>(12), create<LiteralNode>(7));
        Node* port2 = create<PortFieldNode>("another port", "another field");
        Node* root = create<SumNode>(node1, port2);
        return root;
    }

    Node* substitute(Node* original)
    {
        PortFieldSumSubstitutionContext ctx;
        Node* sum1 = create<LiteralNode>(12);
        Node* sum2 = create<LiteralNode>(7);
        std::vector<Node*> v = {sum1, sum2};
        ctx.portfieldSum.emplace(std::piecewise_construct,
                                 std::forward_as_tuple("port", "sum of literal"),
                                 std::forward_as_tuple(v));

        PortFieldSumSubstitutionVisitor sub(*this, ctx);
        return sub.dispatch(original);
    }
};

BOOST_FIXTURE_TEST_CASE(PortFieldSumSubstitutionVisitor_simple, SumSubstitutionFixture)

{
    Node* original = originalExpression();
    Node* substituted = substitute(original);
    Node* expected = expectedExpressionAfterSubstitution();

    CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(substituted, expected));
}

BOOST_FIXTURE_TEST_CASE(PortfieldSumSubstitutionVisitor_name, Registry<Node>)
{
    PortFieldSumSubstitutionContext ctx;

    PortFieldSumSubstitutionVisitor substitutionVisitor(*this, ctx);
    BOOST_CHECK_EQUAL(substitutionVisitor.name(), "PortFieldSumSubstitutionVisitor");
}

BOOST_AUTO_TEST_SUITE_END()
