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
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"

#include "UtilMocks.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy::SystemModel;

BOOST_AUTO_TEST_SUITE(_DeepTree_)

static Node* deepNegationTree(Registry<Node>& registry, double litValue, size_t depth)
{
    Node* node = registry.create<LiteralNode>(litValue);
    for (size_t c = 0; c < depth; c++)
    {
        node = registry.create<NegationNode>(node);
    }
    return node;
}

BOOST_FIXTURE_TEST_CASE(deep_tree_even, MyDummyFixture)
{
    Node* node = deepNegationTree(*this, 42., 1000);
    // (-1)^1000 = 1
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(node).valueAsDouble(), 42.);
}

BOOST_FIXTURE_TEST_CASE(deep_tree_odd, MyDummyFixture)
{
    Node* node = deepNegationTree(*this, 42., 1001);
    // (-1)^1001 = -1
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(node).valueAsDouble(), -42.);
}

static Node* deepAddTree(Registry<Node>& registry, SumNode* root, int depth)
{
    if (depth > 0)
    {
        Node* left = deepAddTree(registry, root, depth - 1);
        Node* right = deepAddTree(registry, root, depth - 1);
        return registry.create<SumNode>(left, right);
    }
    else
    {
        return registry.create<LiteralNode>(42.);
    }
}

BOOST_FIXTURE_TEST_CASE(binary_tree, MyDummyFixture)
{
    // SumNode's children are not mutable, so we'll replace this empty root with an actual one
    SumNode* root = create<SumNode>(nullptr, nullptr);
    Node* node = deepAddTree(*this, root, 10);
    // We expect 1024 = 2^10 literal nodes, each carrying value 42.
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(node).valueAsDouble(), 42. * 1024);
}

BOOST_FIXTURE_TEST_CASE(wide_sum_tree, MyDummyFixture)
{
    const int nb_operands = 1'000;
    std::vector<Node*> operands(nb_operands);
    for (auto& op: operands)
    {
        op = create<LiteralNode>(1.);
    }
    SumNode root(std::move(operands));
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(&root).valueAsDouble(),
                      nb_operands * 1.);
}

BOOST_AUTO_TEST_SUITE_END()
