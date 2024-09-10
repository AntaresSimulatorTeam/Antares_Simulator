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

#include <algorithm>

#include <boost/test/unit_test.hpp>

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/iterators/pre-order.h>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;

BOOST_AUTO_TEST_SUITE(_Iterator_)

static Node* simpleExpression(Registry<Node>& registry)
{
    return registry.create<AddNode>(registry.create<LiteralNode>(2.),
                                    registry.create<LiteralNode>(21.));
}

BOOST_FIXTURE_TEST_CASE(count_literal_nodes_for_loop, Registry<Node>)
{
    Node* root = simpleExpression(*this);
    int count_lit = 0;
    for (auto& node: AST(root))
    {
        if (dynamic_cast<LiteralNode*>(&node))
        {
            count_lit++;
        }
    }
    BOOST_CHECK_EQUAL(count_lit, 2);
}

BOOST_FIXTURE_TEST_CASE(count_literal_nodes_count_if, Registry<Node>)
{
    Node* root = simpleExpression(*this);
    AST ast(root);
    int count_lit = std::count_if(ast.begin(),
                                  ast.end(),
                                  [](Node& node)
                                  { return dynamic_cast<LiteralNode*>(&node) != nullptr; });

    BOOST_CHECK_EQUAL(count_lit, 2);
}

BOOST_FIXTURE_TEST_CASE(find_if_not_found, Registry<Node>)
{
    Node* root = simpleExpression(*this);
    auto* add = dynamic_cast<AddNode*>(root);
    BOOST_REQUIRE(add);

    AST ast(root);
    auto it = std::find_if(ast.begin(),
                           ast.end(),
                           [](Node& node)
                           { return dynamic_cast<MultiplicationNode*>(&node) != nullptr; });
    BOOST_CHECK(it == ast.end());
}

BOOST_FIXTURE_TEST_CASE(find_if_found, Registry<Node>)
{
    Node* root = simpleExpression(*this);
    auto* add = dynamic_cast<AddNode*>(root);
    BOOST_REQUIRE(add);

    AST ast(root);
    auto it = std::find_if(ast.begin(),
                           ast.end(),
                           [](Node& node) { return dynamic_cast<LiteralNode*>(&node) != nullptr; });
    BOOST_CHECK(it != ast.end());
    auto* res = dynamic_cast<LiteralNode*>(&*it);
    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(res->value(), 2.);
}

BOOST_AUTO_TEST_SUITE_END()
