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

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/CompareVisitor.h>
#include <antares/solver/expressions/visitors/PortfieldSubstitutionVisitor.h>
#include <antares/solver/expressions/visitors/PrintVisitor.h>
#include <antares/solver/expressions/visitors/SubstitutionVisitor.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;
using namespace Antares::Solver::Visitors;

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
        PortfieldSubstitutionContext ctx;
        ctx.portfield.emplace(PortFieldNode("port", "literal"), create<LiteralNode>(10));

        PortfieldSubstitutionVisitor sub(*this, ctx);
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
    PortfieldSubstitutionContext ctx;

    PortfieldSubstitutionVisitor substitutionVisitor(*this, ctx);
    BOOST_CHECK_EQUAL(substitutionVisitor.name(), "PortfieldSubstitutionVisitor");
}

BOOST_AUTO_TEST_SUITE_END()
