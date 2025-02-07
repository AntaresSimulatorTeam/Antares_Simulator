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

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;

BOOST_AUTO_TEST_SUITE(_Nodes_)

// BOOST_AUTO_TEST_CASE(UnaryNodeTest)
// {
//     LiteralNode literalNode(23.);
//     UnaryNode unaryNodeWithNullChild(nullptr);
//     BOOST_CHECK_EQUAL(unaryNodeWithNullChild.child(), nullptr);
//     UnaryNode unaryNode(&literalNode);
//     BOOST_CHECK_EQUAL(unaryNode.child(), &literalNode);
// }

BOOST_AUTO_TEST_CASE(PortFieldNodeTest)
{
    LiteralNode literalNode(23.);
    std::string portName1("p1");
    std::string portName2("p2");
    std::string fieldName1("f1");
    std::string fieldName2("f2");

    PortFieldNode portFieldNode1(portName1, fieldName1);
    PortFieldNode portFieldNode1TwinNode(portName1, fieldName1);
    BOOST_CHECK_EQUAL(portFieldNode1 == portFieldNode1TwinNode, true);

    PortFieldNode shareNameWithPortField1(portName1, fieldName2);
    BOOST_CHECK_EQUAL(portFieldNode1 == shareNameWithPortField1, false);

    PortFieldNode shareFieldNameWithPortField1(portName2, fieldName1);
    BOOST_CHECK_EQUAL(portFieldNode1 == shareFieldNameWithPortField1, false);

    PortFieldNode portFieldNode2(portName2, fieldName2);
    BOOST_CHECK_EQUAL(portFieldNode1 == portFieldNode2, false);
}

BOOST_AUTO_TEST_CASE(PortFieldSumNodeTest)
{
    std::string portName1("p1");
    std::string fieldName1("f1");

    PortFieldSumNode pfs(portName1, fieldName1);
    BOOST_CHECK_EQUAL(pfs.getPortName(), portName1);
    BOOST_CHECK_EQUAL(pfs.getFieldName(), fieldName1);
}

BOOST_FIXTURE_TEST_CASE(nodes_name, Registry<Node>)
{
    auto literalNode = create<LiteralNode>(2024.2);
    std::map<Node*, std::string> nodes = {
      {literalNode, "LiteralNode"},
      {create<SumNode>(literalNode, literalNode), "SumNode"},
      {create<SubtractionNode>(literalNode, literalNode), "SubtractionNode"},
      {create<MultiplicationNode>(literalNode, literalNode), "MultiplicationNode"},
      {create<DivisionNode>(literalNode, literalNode), "DivisionNode"},
      {create<EqualNode>(literalNode, literalNode), "EqualNode"},
      {create<LessThanOrEqualNode>(literalNode, literalNode), "LessThanOrEqualNode"},
      {create<GreaterThanOrEqualNode>(literalNode, literalNode), "GreaterThanOrEqualNode"},
      {create<NegationNode>(literalNode), "NegationNode"},
      {create<ComponentVariableNode>(literalNode->name(), literalNode->name()),
       "ComponentVariableNode"},
      {create<ComponentParameterNode>(literalNode->name(), literalNode->name()),
       "ComponentParameterNode"},
      {create<ParameterNode>(literalNode->name()), "ParameterNode"},
      {create<VariableNode>(literalNode->name()), "VariableNode"},
      {create<PortFieldNode>(literalNode->name(), literalNode->name()), "PortFieldNode"},
      {create<PortFieldSumNode>(literalNode->name(), literalNode->name()), "PortFieldSumNode"}};

    for (auto [node, name]: nodes)
    {
        BOOST_CHECK_EQUAL(node->name(), name);
    }
}

BOOST_AUTO_TEST_SUITE_END()
