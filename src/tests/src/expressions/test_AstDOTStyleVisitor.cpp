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
#include <antares/expressions/visitors/AstDOTStyleVisitor.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

BOOST_AUTO_TEST_SUITE(_AstGraphVisitor_)

class Fixture
{
public:
    Fixture()
    {
    }

    Node* makeExpression()
    {
        Node* literalNode = registry_.create<LiteralNode>(-40.);
        Node* negationNode = registry_.create<NegationNode>(literalNode);
        Node* parameterNode = registry_.create<ParameterNode>("avogadro_constant");
        Node* multiplicationNode = registry_.create<MultiplicationNode>(negationNode,
                                                                        parameterNode);
        Node* variableNode = registry_.create<VariableNode>("atoms_count");
        Node* divisionNode = registry_.create<DivisionNode>(variableNode, multiplicationNode);
        Node* portFieldNode = registry_.create<PortFieldNode>("gasStation", "1149");
        Node* portFieldSumNode = registry_.create<PortFieldSumNode>("portfield", "sum");
        Node* sumNode = registry_.create<SumNode>(divisionNode, portFieldNode, portFieldSumNode);
        Node* componentVariableNode = registry_.create<ComponentVariableNode>("1150",
                                                                              "otherStation");
        Node* componentParameterNode = registry_.create<ComponentParameterNode>("1151",
                                                                                "otherConstant");
        Node* subtractionNode = registry_.create<SubtractionNode>(componentVariableNode,
                                                                  componentParameterNode);
        Node* equalNode = registry_.create<EqualNode>(subtractionNode, sumNode);
        Node* literalNode2 = registry_.create<LiteralNode>(53.);
        Node* lessThanOrEqualNode = registry_.create<LessThanOrEqualNode>(literalNode2, equalNode);
        Node* literalNode3 = registry_.create<LiteralNode>(54.);
        Node* greaterThanOrEqualNode = registry_.create<GreaterThanOrEqualNode>(
          literalNode3,
          lessThanOrEqualNode);

        return greaterThanOrEqualNode;
    }

    static std::string expectedDotContent()
    {
        return R"raw(digraph ExpressionTree {
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
  7 -> 17;
  17 [label="PFSUM(portfield,sum)", shape="component", style="filled, solid", color="olive"];
label="AST Diagram(Total nodes : 17)"
labelloc = "t"
subgraph cluster_legend {
label = "Legend";
style = dashed;
fontsize = 16;
color = lightgrey;
node [shape=plaintext];

legend_ComponentParameterNode [ label =" ComponentParameterNode: 1"]
legend_ComponentParameterNode -> legend_ComponentVariableNode [style=invis];
legend_ComponentVariableNode [ label =" ComponentVariableNode: 1"]
legend_ComponentVariableNode -> legend_DivisionNode [style=invis];
legend_DivisionNode [ label =" DivisionNode: 1"]
legend_DivisionNode -> legend_EqualNode [style=invis];
legend_EqualNode [ label =" EqualNode: 1"]
legend_EqualNode -> legend_GreaterThanOrEqualNode [style=invis];
legend_GreaterThanOrEqualNode [ label =" GreaterThanOrEqualNode: 1"]
legend_GreaterThanOrEqualNode -> legend_LessThanOrEqualNode [style=invis];
legend_LessThanOrEqualNode [ label =" LessThanOrEqualNode: 1"]
legend_LessThanOrEqualNode -> legend_LiteralNode [style=invis];
legend_LiteralNode [ label =" LiteralNode: 3"]
legend_LiteralNode -> legend_MultiplicationNode [style=invis];
legend_MultiplicationNode [ label =" MultiplicationNode: 1"]
legend_MultiplicationNode -> legend_NegationNode [style=invis];
legend_NegationNode [ label =" NegationNode: 1"]
legend_NegationNode -> legend_ParameterNode [style=invis];
legend_ParameterNode [ label =" ParameterNode: 1"]
legend_ParameterNode -> legend_PortFieldNode [style=invis];
legend_PortFieldNode [ label =" PortFieldNode: 1"]
legend_PortFieldNode -> legend_PortFieldSumNode [style=invis];
legend_PortFieldSumNode [ label =" PortFieldSumNode: 1"]
legend_PortFieldSumNode -> legend_SubtractionNode [style=invis];
legend_SubtractionNode [ label =" SubtractionNode: 1"]
legend_SubtractionNode -> legend_SumNode [style=invis];
legend_SumNode [ label =" SumNode: 1"]
legend_SumNode -> legend_VariableNode [style=invis];
legend_VariableNode [ label =" VariableNode: 1"]
}
}
)raw";
    }

    Registry<Node> registry_;
};

BOOST_FIXTURE_TEST_CASE(
  dot_visitor_is_run_on_complex_expression___resulting_dot_content_as_expected,
  Fixture)
{
    std::ostringstream dotContentStream;

    AstDOTStyleVisitor astGraphVisitor;
    astGraphVisitor(dotContentStream, makeExpression());

    BOOST_CHECK_EQUAL(dotContentStream.str(), expectedDotContent());
}

BOOST_FIXTURE_TEST_CASE(AstDOTStyleVisitor_name, Registry<Node>)
{
    AstDOTStyleVisitor astGraphVisitor;
    BOOST_CHECK_EQUAL(astGraphVisitor.name(), "AstDOTStyleVisitor");
}

BOOST_AUTO_TEST_SUITE_END()
