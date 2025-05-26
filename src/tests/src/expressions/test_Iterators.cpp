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
#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;

BOOST_AUTO_TEST_SUITE(_Iterator_)

static Node* simpleExpression(Registry<Node>& registry)
{
    return registry.create<SumNode>(registry.create<LiteralNode>(2.),
                                    registry.create<LiteralNode>(21.));
}

BOOST_FIXTURE_TEST_CASE(empty_ast_begin_is_end, Registry<Node>)
{
    AST ast(nullptr);
    BOOST_CHECK(ast.begin() == ast.end());
}

BOOST_FIXTURE_TEST_CASE(simple_end_is_end, Registry<Node>)
{
    AST ast(create<LiteralNode>(32.));
    BOOST_CHECK(ast.end() == ast.end());
}

BOOST_FIXTURE_TEST_CASE(dereference_op, Registry<Node>)
{
    AST ast(create<LiteralNode>(21.));
    auto it = ast.begin();
    const std::string expected("LiteralNode");
    BOOST_CHECK_EQUAL(it->name(), expected);
    BOOST_CHECK_EQUAL((*it).name(), expected);
}

BOOST_FIXTURE_TEST_CASE(unary_dereference, Registry<Node>)
{
    AST ast(create<NegationNode>(nullptr));
    auto it = ast.begin();
    BOOST_CHECK(!it->name().empty());
    BOOST_CHECK(!(*it).name().empty());
}

BOOST_FIXTURE_TEST_CASE(count_literal_nodes_for_loop, Registry<Node>)
{
    int count_lit = 0;
    for (auto& node: AST(simpleExpression(*this)))
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
    AST ast(simpleExpression(*this));
    int count_lit = std::count_if(ast.begin(),
                                  ast.end(),
                                  [](Node& node)
                                  { return dynamic_cast<LiteralNode*>(&node) != nullptr; });

    BOOST_CHECK_EQUAL(count_lit, 2);
}

BOOST_FIXTURE_TEST_CASE(find_if_not_found, Registry<Node>)
{
    AST ast(simpleExpression(*this));
    auto it = std::find_if(ast.begin(),
                           ast.end(),
                           [](Node& node)
                           { return dynamic_cast<MultiplicationNode*>(&node) != nullptr; });
    BOOST_CHECK(it == ast.end());
}

BOOST_FIXTURE_TEST_CASE(find_if_found, Registry<Node>)
{
    AST ast(simpleExpression(*this));
    auto it = std::find_if(ast.begin(),
                           ast.end(),
                           [](Node& node) { return dynamic_cast<LiteralNode*>(&node) != nullptr; });
    BOOST_CHECK(it != ast.end());
    auto* res = dynamic_cast<LiteralNode*>(&*it);
    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(res->value(), 2.);
}

BOOST_FIXTURE_TEST_CASE(distance_is_3, Registry<Node>)
{
    AST ast(simpleExpression(*this));
    BOOST_CHECK_EQUAL(std::distance(ast.begin(), ast.end()), 3);
}

BOOST_FIXTURE_TEST_CASE(distance_unary, Registry<Node>)
{
    AST ast(create<NegationNode>(create<NegationNode>(create<LiteralNode>(32.))));
    BOOST_CHECK_EQUAL(std::distance(ast.begin(), ast.end()), 3);
}

BOOST_FIXTURE_TEST_CASE(distance_nullptr_is_3, Registry<Node>)
{
    AST ast(create<SumNode>(nullptr, create<LiteralNode>(2.)));
    BOOST_CHECK_EQUAL(std::distance(ast.begin(), ast.end()), 3);
}

BOOST_AUTO_TEST_SUITE_END()
