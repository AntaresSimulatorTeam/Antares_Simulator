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

    Node* makeBiggerExpression()
    {
        auto gt = makeExpression();
        Node* literalNode2 = registry_.create<LiteralNode>(-34);
        Node* literalNode3 = registry_.create<LiteralNode>(12.40);
        Node* parameterNode = registry_.create<ParameterNode>("Planck's constant");
        Node* timeShiftNode = registry_.create<TimeShiftNode>(parameterNode, literalNode2);
        Node* literalNode = registry_.create<LiteralNode>(6.62607015);
        Node* parm2 = registry_.create<ParameterNode>("par");
        Node* timeIndexNode = registry_.create<TimeIndexNode>(
          literalNode,
          registry_.create<SumNode>(parm2, literalNode3));

        Node* parameterNode2 = registry_.create<ParameterNode>("div");
        Node* lit2 = registry_.create<LiteralNode>(365);
        Node* mu2 = registry_.create<MultiplicationNode>(parameterNode2, lit2);
        Node* alltimeSimNode = registry_.create<AllTimeSumNode>(mu2);

        Node* from = registry_.create<ParameterNode>("from");

        Node* parameterNode3 = registry_.create<ParameterNode>("temp");
        Node* lit3 = registry_.create<LiteralNode>(658);
        Node* to = registry_.create<ParameterNode>("to");
        Node* div = registry_.create<DivisionNode>(parameterNode3, lit3);
        Node* timeSumNode = registry_.create<TimeSumNode>(from, to, div);

        return registry_.create<SumNode>(gt,
                                         timeIndexNode,
                                         timeShiftNode,
                                         timeSumNode,
                                         alltimeSimNode);
    }

    static std::string expectedDotContent()
    {
        return R"raw(digraph ExpressionTree {
node[style = filled]
  1 [label=">=", shape="diamond", style="filled", color="yellow"];
  1 -> 2;
  2 [label="54.000000", shape="box", style="filled, solid", color="lightgray"];
  1 -> 3;
  3 [label="<=", shape="diamond", style="filled", color="yellow"];
  3 -> 4;
  4 [label="53.000000", shape="box", style="filled, solid", color="lightgray"];
  3 -> 5;
  5 [label="==", shape="diamond", style="filled", color="yellow"];
  5 -> 6;
  6 [label="-", shape="oval", style="filled, rounded", color="aqua"];
  6 -> 7;
  7 [label="CV(1150,otherStation)", shape="octagon", style="filled, solid", color="goldenrod"];
  6 -> 8;
  8 [label="CP(1151,otherConstant)", shape="octagon", style="filled, solid", color="springgreen"];
  5 -> 9;
  9 [label="+", shape="hexagon", style="filled, solid", color="aqua"];
  9 -> 10;
  10 [label="/", shape="oval", style="filled, rounded", color="aqua"];
  10 -> 11;
  11 [label="Var(atoms_count)", shape="box", style="filled, solid", color="gold"];
  10 -> 12;
  12 [label="*", shape="oval", style="filled, rounded", color="aqua"];
  12 -> 13;
  13 [label="-", shape="invtriangle", style="filled, solid", color="tomato"];
  13 -> 14;
  14 [label="-40.000000", shape="box", style="filled, solid", color="lightgray"];
  12 -> 15;
  15 [label="Param(avogadro_constant)", shape="box", style="filled, solid", color="wheat"];
  9 -> 16;
  16 [label="PF(gasStation,1149)", shape="component", style="filled, solid", color="olive"];
  9 -> 17;
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

    static std::string expectedForBiggerDotContent()
    {
        return R"raw(digraph ExpressionTree {
node[style = filled]
  1 [label="+", shape="hexagon", style="filled, solid", color="aqua"];
  1 -> 2;
  2 [label=">=", shape="diamond", style="filled", color="yellow"];
  2 -> 3;
  3 [label="54.000000", shape="box", style="filled, solid", color="lightgray"];
  2 -> 4;
  4 [label="<=", shape="diamond", style="filled", color="yellow"];
  4 -> 5;
  5 [label="53.000000", shape="box", style="filled, solid", color="lightgray"];
  4 -> 6;
  6 [label="==", shape="diamond", style="filled", color="yellow"];
  6 -> 7;
  7 [label="-", shape="oval", style="filled, rounded", color="aqua"];
  7 -> 8;
  8 [label="CV(1150,otherStation)", shape="octagon", style="filled, solid", color="goldenrod"];
  7 -> 9;
  9 [label="CP(1151,otherConstant)", shape="octagon", style="filled, solid", color="springgreen"];
  6 -> 10;
  10 [label="+", shape="hexagon", style="filled, solid", color="aqua"];
  10 -> 11;
  11 [label="/", shape="oval", style="filled, rounded", color="aqua"];
  11 -> 12;
  12 [label="Var(atoms_count)", shape="box", style="filled, solid", color="gold"];
  11 -> 13;
  13 [label="*", shape="oval", style="filled, rounded", color="aqua"];
  13 -> 14;
  14 [label="-", shape="invtriangle", style="filled, solid", color="tomato"];
  14 -> 15;
  15 [label="-40.000000", shape="box", style="filled, solid", color="lightgray"];
  13 -> 16;
  16 [label="Param(avogadro_constant)", shape="box", style="filled, solid", color="wheat"];
  10 -> 17;
  17 [label="PF(gasStation,1149)", shape="component", style="filled, solid", color="olive"];
  10 -> 18;
  18 [label="PFSUM(portfield,sum)", shape="component", style="filled, solid", color="olive"];
  1 -> 19;
  19 [label="[]", shape="diamond", style="filled", color="gold"];
  19 -> 20;
  20 [label="6.626070", shape="box", style="filled, solid", color="lightgray"];
  19 -> 21;
  21 [label="+", shape="hexagon", style="filled, solid", color="aqua"];
  21 -> 22;
  22 [label="Param(par)", shape="box", style="filled, solid", color="wheat"];
  21 -> 23;
  23 [label="12.400000", shape="box", style="filled, solid", color="lightgray"];
  1 -> 24;
  24 [label="[t]", shape="hexagon", style="filled, solid", color="aqua"];
  24 -> 25;
  25 [label="Param(Planck's constant)", shape="box", style="filled, solid", color="wheat"];
  24 -> 26;
  26 [label="-34.000000", shape="box", style="filled, solid", color="lightgray"];
  1 -> 27;
  27 [label="sum[t]", shape="hexagon", style="filled, solid", color="aqua"];
  27 -> 28;
  28 [label="Param(from)", shape="box", style="filled, solid", color="wheat"];
  27 -> 29;
  29 [label="Param(to)", shape="box", style="filled, solid", color="wheat"];
  27 -> 30;
  30 [label="/", shape="oval", style="filled, rounded", color="aqua"];
  30 -> 31;
  31 [label="Param(temp)", shape="box", style="filled, solid", color="wheat"];
  30 -> 32;
  32 [label="658.000000", shape="box", style="filled, solid", color="lightgray"];
  1 -> 33;
  33 [label="sum[]", shape="diamond", style="filled", color="gold"];
  33 -> 34;
  34 [label="*", shape="oval", style="filled, rounded", color="aqua"];
  34 -> 35;
  35 [label="Param(div)", shape="box", style="filled, solid", color="wheat"];
  34 -> 36;
  36 [label="365.000000", shape="box", style="filled, solid", color="lightgray"];
label="AST Diagram(Total nodes : 36)"
labelloc = "t"
subgraph cluster_legend {
label = "Legend";
style = dashed;
fontsize = 16;
color = lightgrey;
node [shape=plaintext];

legend_AllTimeSumNode [ label =" AllTimeSumNode: 1"]
legend_AllTimeSumNode -> legend_ComponentParameterNode [style=invis];
legend_ComponentParameterNode [ label =" ComponentParameterNode: 1"]
legend_ComponentParameterNode -> legend_ComponentVariableNode [style=invis];
legend_ComponentVariableNode [ label =" ComponentVariableNode: 1"]
legend_ComponentVariableNode -> legend_DivisionNode [style=invis];
legend_DivisionNode [ label =" DivisionNode: 2"]
legend_DivisionNode -> legend_EqualNode [style=invis];
legend_EqualNode [ label =" EqualNode: 1"]
legend_EqualNode -> legend_GreaterThanOrEqualNode [style=invis];
legend_GreaterThanOrEqualNode [ label =" GreaterThanOrEqualNode: 1"]
legend_GreaterThanOrEqualNode -> legend_LessThanOrEqualNode [style=invis];
legend_LessThanOrEqualNode [ label =" LessThanOrEqualNode: 1"]
legend_LessThanOrEqualNode -> legend_LiteralNode [style=invis];
legend_LiteralNode [ label =" LiteralNode: 8"]
legend_LiteralNode -> legend_MultiplicationNode [style=invis];
legend_MultiplicationNode [ label =" MultiplicationNode: 2"]
legend_MultiplicationNode -> legend_NegationNode [style=invis];
legend_NegationNode [ label =" NegationNode: 1"]
legend_NegationNode -> legend_ParameterNode [style=invis];
legend_ParameterNode [ label =" ParameterNode: 7"]
legend_ParameterNode -> legend_PortFieldNode [style=invis];
legend_PortFieldNode [ label =" PortFieldNode: 1"]
legend_PortFieldNode -> legend_PortFieldSumNode [style=invis];
legend_PortFieldSumNode [ label =" PortFieldSumNode: 1"]
legend_PortFieldSumNode -> legend_SubtractionNode [style=invis];
legend_SubtractionNode [ label =" SubtractionNode: 1"]
legend_SubtractionNode -> legend_SumNode [style=invis];
legend_SumNode [ label =" SumNode: 3"]
legend_SumNode -> legend_TimeIndexNode [style=invis];
legend_TimeIndexNode [ label =" TimeIndexNode: 1"]
legend_TimeIndexNode -> legend_TimeShiftNode [style=invis];
legend_TimeShiftNode [ label =" TimeShiftNode: 1"]
legend_TimeShiftNode -> legend_TimeSumNode [style=invis];
legend_TimeSumNode [ label =" TimeSumNode: 1"]
legend_TimeSumNode -> legend_VariableNode [style=invis];
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

BOOST_FIXTURE_TEST_CASE(
  dot_visitor_is_run_on_other_complex_expression___resulting_dot_content_as_expected,
  Fixture)
{
    std::ostringstream dotContentStream;

    AstDOTStyleVisitor astGraphVisitor;
    astGraphVisitor(dotContentStream, makeBiggerExpression());

    BOOST_CHECK_EQUAL(dotContentStream.str(), expectedForBiggerDotContent());
}

BOOST_FIXTURE_TEST_CASE(AstDOTStyleVisitor_name, Registry<Node>)
{
    AstDOTStyleVisitor astGraphVisitor;
    BOOST_CHECK_EQUAL(astGraphVisitor.name(), "AstDOTStyleVisitor");
}

BOOST_AUTO_TEST_SUITE_END()
