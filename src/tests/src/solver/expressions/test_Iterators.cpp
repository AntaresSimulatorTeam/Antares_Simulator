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
#include <antares/solver/expressions/iterators/pre-order.h>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;

BOOST_AUTO_TEST_SUITE(_Iterator_)

BOOST_FIXTURE_TEST_CASE(deep_tree_even, Registry<Node>)
{
    Node* root = create<AddNode>(create<LiteralNode>(2.), create<LiteralNode>(21.));
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

BOOST_AUTO_TEST_SUITE_END()
