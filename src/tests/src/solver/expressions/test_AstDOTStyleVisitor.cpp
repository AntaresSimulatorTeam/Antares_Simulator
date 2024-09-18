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
#include <antares/solver/expressions/visitors/AstDOTStyleVisitor.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;
using namespace Antares::Solver::Visitors;

BOOST_AUTO_TEST_SUITE(_AstGraphVisitor_)

static auto buildAllTypeNode(Registry<Node>& registry)
{
    const std::string expected_msg = R"whenThemusicIsOverTurnOuttheLights(digraph ExpressionTree {
node[style = filled]
  1 [label=">=", shape="diamond", style="filled", color="yellow"];
  1 -> 2;
  1 -> 3;
  2 [label="54.000000", shape="box", style="filled, solid", color="lightgray"];
  3 [label="<=", shape="diamond", style="filled", color="yellow"];
  3 -> 4;
  3 -> 5;
  4 [label="53.000000", shape="box", style="filled, solid", color="lightgray"];
  5 [label="==", shape="diamond", style="filled", color="yellow"];
  5 -> 6;
  5 -> 7;
  6 [label="-", shape="oval", style="filled, rounded", color="aqua"];
  6 -> 8;
  6 -> 9;
  8 [label="CV(1150,otherStation)", shape="octagon", style="filled, solid", color="goldenrod"];
  9 [label="CP(1151,otherConstant)", shape="octagon", style="filled, solid", color="springgreen"];
  7 [label="+", shape="hexagon", style="filled, solid", color="aqua"];
  7 -> 10;
  10 [label="/", shape="oval", style="filled, rounded", color="aqua"];
  10 -> 11;
  10 -> 12;
  11 [label="Var(atoms_count)", shape="box", style="filled, solid", color="gold"];
  12 [label="*", shape="oval", style="filled, rounded", color="aqua"];
  12 -> 13;
  12 -> 14;
  13 [label="-", shape="invtriangle", style="filled, solid", color="tomato"];
  13 -> 15;
  15 [label="-40.000000", shape="box", style="filled, solid", color="lightgray"];
  14 [label="Param(avogadro_constant)", shape="box", style="filled, solid", color="wheat"];
  7 -> 16;
  16 [label="PF(gasStation,1149)", shape="component", style="filled, solid", color="olive"];
label="AST Diagram(Total nodes : 16)"
labelloc = "t"
subgraph cluster_legend {
label = "Legend";
style = dashed;
fontsize = 16;
color = lightgrey;
node [shape=plaintext];

legend_SubtractionNode [ label =" SubtractionNode: 1"]
legend_GreaterThanOrEqualNode [ label =" GreaterThanOrEqualNode: 1"]
legend_LiteralNode [ label =" LiteralNode: 3"]
legend_LessThanOrEqualNode [ label =" LessThanOrEqualNode: 1"]
legend_ComponentParameterNode [ label =" ComponentParameterNode: 1"]
legend_EqualNode [ label =" EqualNode: 1"]
legend_ComponentVariableNode [ label =" ComponentVariableNode: 1"]
legend_SumNode [ label =" SumNode: 1"]
legend_DivisionNode [ label =" DivisionNode: 1"]
legend_VariableNode [ label =" VariableNode: 1"]
legend_MultiplicationNode [ label =" MultiplicationNode: 1"]
legend_NegationNode [ label =" NegationNode: 1"]
legend_ParameterNode [ label =" ParameterNode: 1"]
legend_PortFieldNode [ label =" PortFieldNode: 1"]
legend_SubtractionNode -> legend_GreaterThanOrEqualNode[style=invis];
legend_GreaterThanOrEqualNode -> legend_LiteralNode[style=invis];
legend_LiteralNode -> legend_LessThanOrEqualNode[style=invis];
legend_LessThanOrEqualNode -> legend_ComponentParameterNode[style=invis];
legend_ComponentParameterNode -> legend_EqualNode[style=invis];
legend_EqualNode -> legend_ComponentVariableNode[style=invis];
legend_ComponentVariableNode -> legend_SumNode[style=invis];
legend_SumNode -> legend_DivisionNode[style=invis];
legend_DivisionNode -> legend_VariableNode[style=invis];
legend_VariableNode -> legend_MultiplicationNode[style=invis];
legend_MultiplicationNode -> legend_NegationNode[style=invis];
legend_NegationNode -> legend_ParameterNode[style=invis];
legend_ParameterNode -> legend_PortFieldNode[style=invis];
}
}
)whenThemusicIsOverTurnOuttheLights";

    Node* literalNode = registry.create<LiteralNode>(-40.);
    Node* negationNode = registry.create<NegationNode>(literalNode);
    Node* parameterNode = registry.create<ParameterNode>("avogadro_constant");
    Node* multiplicationNode = registry.create<MultiplicationNode>(negationNode, parameterNode);
    Node* variableNode = registry.create<VariableNode>("atoms_count");
    Node* divisionNode = registry.create<DivisionNode>(variableNode, multiplicationNode);
    Node* portFieldNode = registry.create<PortFieldNode>("gasStation", "1149");
    Node* sumNode = registry.create<SumNode>(divisionNode, portFieldNode);
    Node* componentVariableNode = registry.create<ComponentVariableNode>("1150", "otherStation");
    Node* componentParameterNode = registry.create<ComponentParameterNode>("1151", "otherConstant");
    Node* subtractionNode = registry.create<SubtractionNode>(componentVariableNode,
                                                             componentParameterNode);
    Node* equalNode = registry.create<EqualNode>(subtractionNode, sumNode);
    Node* literalNode2 = registry.create<LiteralNode>(53.);
    Node* lessThanOrEqualNode = registry.create<LessThanOrEqualNode>(literalNode2, equalNode);
    Node* literalNode3 = registry.create<LiteralNode>(54.);
    Node* greaterThanOrEqualNode = registry.create<GreaterThanOrEqualNode>(literalNode3,
                                                                           lessThanOrEqualNode);

    return std::pair{expected_msg, greaterThanOrEqualNode};
}

BOOST_FIXTURE_TEST_CASE(tree_with_all_type_node, Registry<Node>)
{
    auto [expected_msg, expr] = buildAllTypeNode(*this);

    const auto filename = std::filesystem::temp_directory_path() / "out.dot";
    std::ofstream out(filename);
    std::ostringstream os3;
    std::vector<std::ostream*> ostreams{&out, &std::cout, &os3};
    std::stringstream redirectedStdout;
    std::streambuf* initialBufferCout = std::cout.rdbuf(redirectedStdout.rdbuf());

    AstDOTStyleVisitor astGraphVisitor;
    for (auto* os: ostreams)
    {
        std::pair<AstDOTStyleVisitor&, Node*> pair1(astGraphVisitor, expr);

        *os << pair1;
    }
    std::cout.rdbuf(initialBufferCout);
    out.close();

    // read what was written in stdout
    BOOST_CHECK_EQUAL(expected_msg, redirectedStdout.str());
    // read out.dot content
    std::ifstream infile(filename);
    std::string out_content((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
    BOOST_CHECK_EQUAL(expected_msg, out_content);

    // read the content of os3
    BOOST_CHECK_EQUAL(expected_msg, os3.str());
}

BOOST_AUTO_TEST_SUITE_END()
