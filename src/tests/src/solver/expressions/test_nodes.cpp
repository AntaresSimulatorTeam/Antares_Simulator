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

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;

BOOST_AUTO_TEST_SUITE(_Nodes_)

// BOOST_AUTO_TEST_CASE(UnaryNodeTest)
// {
//     LiteralNode literalNode(23.);
//     UnaryNode unaryNodeWithNullChild(nullptr);
//     BOOST_CHECK_EQUAL(unaryNodeWithNullChild.child(), nullptr);
//     UnaryNode unaryNode(&literalNode);
//     BOOST_CHECK_EQUAL(unaryNode.child(), &literalNode);
// }

namespace Antares::Solver::Nodes
{
bool operator!=(const PortFieldNode& rhs, const PortFieldNode& lhs)
{
    return !(rhs == lhs);
}
} // namespace Antares::Solver::Nodes

BOOST_AUTO_TEST_CASE(PortFieldNodeTest)
{
    LiteralNode literalNode(23.);
    std::string portName1("p1");
    std::string portName2("p2");
    std::string fieldName1("f1");
    std::string fieldName2("f2");

    PortFieldNode portFieldNode1(portName1, fieldName1);
    PortFieldNode portFieldNode1TwinNode(portName1, fieldName1);
    BOOST_CHECK(portFieldNode1 == portFieldNode1TwinNode);

    PortFieldNode shareNameWithPortField1(portName1, fieldName2);
    BOOST_CHECK(portFieldNode1 != shareNameWithPortField1);

    PortFieldNode shareFieldNameWithPortField1(portName2, fieldName1);
    BOOST_CHECK(portFieldNode1 != shareFieldNameWithPortField1);

    PortFieldNode portFieldNode2(portName2, fieldName2);
    BOOST_CHECK(portFieldNode1 != portFieldNode2);
}
BOOST_AUTO_TEST_SUITE_END()
