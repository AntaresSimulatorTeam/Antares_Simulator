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
    Node* addNode = create<AddNode>(divisionNode, portFieldNode);
    Node* componentVariableNode = create<ComponentVariableNode>("1150", "otherStation");
    Node* componentParameterNode = create<ComponentParameterNode>("1151", "otherConstant");
    Node* subtractionNode = create<SubtractionNode>(componentVariableNode, componentParameterNode);
    Node* equalNode = create<EqualNode>(subtractionNode, addNode);
    Node* literalNode2 = create<LiteralNode>(53.);
    Node* lessThanOrEqualNode = create<LessThanOrEqualNode>(literalNode2, equalNode);
    Node* literalNode3 = create<LiteralNode>(54.);
    Node* greaterThanOrEqualNode = create<GreaterThanOrEqualNode>(literalNode3,
                                                                  lessThanOrEqualNode);
    std::ofstream out("out.dot");
    AstGraphVisitor astGraphVisitor(&out);
    astGraphVisitor.NewTreeGraph("GasStation");
    astGraphVisitor.dispatch(greaterThanOrEqualNode);
    astGraphVisitor.EndTreeGraph();
    out.close();

    //    BOOST_CHECK_EQUAL(printed1, printed2); // TODO Number of decimals implementation dependent
    //    ?
}

BOOST_FIXTURE_TEST_CASE(change_outstream, Registry<Node>)
{
    Node* literalNode = create<LiteralNode>(-40.);
    Node* negationNode = create<NegationNode>(literalNode);

    std::ofstream out1("out1.dot");
    AstGraphVisitor astGraphVisitor(&out1);
    astGraphVisitor.NewTreeGraph("NegationOperation");
    astGraphVisitor.dispatch(negationNode);
    astGraphVisitor.EndTreeGraph();
    out1.close();

    //-----------------------------------------------//

    Node* parameterNode = create<ParameterNode>("avogadro_constant");
    Node* multiplicationNode = create<MultiplicationNode>(negationNode, parameterNode);
    std::ofstream out2("out2.dot");
    astGraphVisitor.setOutStream(&out2);
    astGraphVisitor.NewTreeGraph("MultiplicationOperation");
    astGraphVisitor.dispatch(multiplicationNode);
    astGraphVisitor.EndTreeGraph();
    out1.close();

    //    BOOST_CHECK_EQUAL(printed1, printed2); // TODO Number of decimals implementation dependent
    //    ?
}
BOOST_AUTO_TEST_SUITE_END()
