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

#include <fstream>
#include <variant>

#include <boost/test/unit_test.hpp>

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/AstGraphVisitor.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;
using namespace Antares::Solver::Visitors;

BOOST_AUTO_TEST_SUITE(_AstGraphVisitor_)

BOOST_FIXTURE_TEST_CASE(tree_with_all_type_node, Registry<Node>)
{
    Node* literalNode = create<LiteralNode>(-40.);
    Node* negationNode = create<NegationNode>(literalNode);
    Node* parameterNode = create<ParameterNode>("avogadro_constant");
    Node* multiplicationNode = create<MultiplicationNode>(negationNode, parameterNode);
    Node* variableNode = create<VariableNode>("atoms_count");
    Node* divisionNode = create<DivisionNode>(variableNode, multiplicationNode);
    Node* portFieldNode = create<PortFieldNode>("gasStation", "1149");
    Node* sumNode = create<SumNode>(divisionNode, portFieldNode);
    Node* componentVariableNode = create<ComponentVariableNode>("1150", "otherStation");
    Node* componentParameterNode = create<ComponentParameterNode>("1151", "otherConstant");
    Node* subtractionNode = create<SubtractionNode>(componentVariableNode, componentParameterNode);
    Node* equalNode = create<EqualNode>(subtractionNode, sumNode);
    Node* literalNode2 = create<LiteralNode>(53.);
    Node* lessThanOrEqualNode = create<LessThanOrEqualNode>(literalNode2, equalNode);
    Node* literalNode3 = create<LiteralNode>(54.);
    Node* greaterThanOrEqualNode = create<GreaterThanOrEqualNode>(literalNode3,
                                                                  lessThanOrEqualNode);

    std::ofstream out("out.dot");
    std::vector<std::ostream*> ostreams{&out, &std::cout};
    std::stringstream redirectedStdout;
    std::streambuf* initialBufferCout = std::cout.rdbuf(redirectedStdout.rdbuf());

    AstGraphVisitor astGraphVisitor;
    for (auto* os: ostreams)
    {
        std::pair<AstGraphVisitor&, Node*> pair1(astGraphVisitor, greaterThanOrEqualNode);

        *os << pair1;
    }
    std::cout.rdbuf(initialBufferCout);
    out.close();

    const std::string expected_msg = R"raw(digraph ExpressionTree {
node[style = filled]
  1 [label=">=", shape="oval", style="filled, rounded", color="beige"];
  1 -> 2;
  1 -> 3;
  2 [label="54.000000", shape="box", style="filled, solid", color="lightcyan"];
  3 [label="<=", shape="oval", style="filled, rounded", color="beige"];
  3 -> 4;
  3 -> 5;
  4 [label="53.000000", shape="box", style="filled, solid", color="lightcyan"];
  5 [label="==", shape="oval", style="filled, rounded", color="beige"];
  5 -> 6;
  5 -> 7;
  6 [label="-", shape="oval", style="filled, rounded", color="moccasin"];
  6 -> 8;
  6 -> 9;
  8 [label="CV(1150,otherStation)", shape="box", style="filled, solid", color="goldenrod"];
  9 [label="CP(1151,otherConstant)", shape="box", style="filled, solid", color="palegreen"];
  7 [label="+", shape="hexagon", style="filled, solid", color="aqua"];
  7 -> 10;
  10 [label="/", shape="oval", style="filled, rounded", color="moccasin"];
  10 -> 11;
  10 -> 12;
  11 [label="Var(atoms_count)", shape="box", style="filled, solid", color="gold"];
  12 [label="*", shape="oval", style="filled, rounded", color="moccasin"];
  12 -> 13;
  12 -> 14;
  13 [label="-", shape="square", style="filled, solid", color="navajowhite"];
  13 -> 15;
  15 [label="-40.000000", shape="box", style="filled, solid", color="lightcyan"];
  14 [label="Param(avogadro_constant)", shape="box", style="filled, solid", color="palegreen"];
  7 -> 16;
  16 [label="PF(gasStation,1149)", shape="invtriangle", style="filled, solid", color="powderblue"];
}
)raw";

    BOOST_CHECK_EQUAL(expected_msg, redirectedStdout.str());
    // read out.dot content
    std::ifstream infile("out.dot");
    std::string out_content((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
    BOOST_CHECK_EQUAL(expected_msg, out_content);
}

BOOST_AUTO_TEST_SUITE_END()
