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
#include <antares/expressions/visitors/CloneVisitor.h>
#include <antares/expressions/visitors/PrintVisitor.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

BOOST_AUTO_TEST_SUITE(_CloneVisitor_)

BOOST_FIXTURE_TEST_CASE(cloneVisitor_With_Add_Neg_ComponentVariableNode, Registry<Node>)
{
    std::string cpvar_name("var"), cpvar_id("id1");
    std::string cp_para_name("par"), cp_para_id("id2");
    ComponentVariableNode cpv(cpvar_id, cpvar_name);
    ComponentParameterNode cpp(cp_para_id, cp_para_name);
    double num1 = 22.0, num2 = 8., num3 = 77.;
    // (num1+num2)
    Node* edge = create<SumNode>(create<LiteralNode>(num1),
                                 create<LiteralNode>(num2),
                                 create<LiteralNode>(num3));
    // -((num1+num2+num3))
    Node* negative_edge = create<NegationNode>(edge);
    // (-((num1+num2+num3))+id1.var)
    Node* add_node = create<SumNode>(negative_edge, &cpv);
    // (-((-((num1+num2+num3))+id1.var))+id2.par) ==
    // (-((-((22.000000+8.000000+77.000000))+id1.var))+id2.par)
    Node* root = create<SumNode>(create<NegationNode>(add_node), &cpp);

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(root);

    BOOST_CHECK_EQUAL(printed, "(-((-((22.000000+8.000000+77.000000))+id1.var))+id2.par)");
    CloneVisitor cloneVisitor(*this);
    Node* cloned = cloneVisitor.dispatch(root);
    BOOST_CHECK_EQUAL(printed, printVisitor.dispatch(cloned));
}

BOOST_FIXTURE_TEST_CASE(clone_TimeShiftNode, Registry<Node>)
{
    LiteralNode literal_node(35.);
    LiteralNode literal_node2(-546.);
    const TimeShiftNode node(&literal_node, &literal_node2);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&node);
    const auto cloneTimeShift = dynamic_cast<TimeShiftNode*>(clone);
    BOOST_REQUIRE(cloneTimeShift);
    BOOST_CHECK(dynamic_cast<LiteralNode*>(cloneTimeShift->right())->value() == -546);

    const auto cloneChild = dynamic_cast<LiteralNode*>(cloneTimeShift->left());
    BOOST_REQUIRE(cloneChild);

    BOOST_CHECK(cloneChild->value() == 35);
}

BOOST_FIXTURE_TEST_CASE(clone_TimeIndexNode, Registry<Node>)
{
    LiteralNode literal_node(35.);
    ParameterNode parameter_node("yolo");
    const TimeIndexNode node(&literal_node, &parameter_node);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&node);
    const auto cloneTimeIndex = dynamic_cast<TimeIndexNode*>(clone);

    BOOST_REQUIRE(cloneTimeIndex);
    const auto leftChild = dynamic_cast<LiteralNode*>(cloneTimeIndex->left());
    BOOST_REQUIRE(leftChild);
    BOOST_CHECK(leftChild->value() == 35);

    const auto rightChild = dynamic_cast<ParameterNode*>(cloneTimeIndex->right());
    BOOST_REQUIRE(rightChild);
    BOOST_CHECK(rightChild->value() == "yolo");
}

BOOST_FIXTURE_TEST_CASE(clone_TimeSumNode, Registry<Node>)
{
    LiteralNode from(35.);
    ParameterNode to("yolo");
    ParameterNode expr("da");
    const TimeSumNode node(&from, &to, &expr);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&node);
    const auto cloneTimeSum = dynamic_cast<TimeSumNode*>(clone);

    BOOST_REQUIRE(cloneTimeSum);
    const auto fromChild = dynamic_cast<LiteralNode*>(cloneTimeSum->from());
    BOOST_REQUIRE(fromChild);
    BOOST_CHECK(fromChild->value() == 35);

    const auto toChild = dynamic_cast<ParameterNode*>(cloneTimeSum->to());
    BOOST_REQUIRE(toChild);
    BOOST_CHECK(toChild->value() == "yolo");
    const auto expression = dynamic_cast<ParameterNode*>(cloneTimeSum->expression());
    BOOST_REQUIRE(expression);
    BOOST_CHECK(expression->value() == "da");
}

BOOST_FIXTURE_TEST_CASE(clone_AllTimeSumNode, Registry<Node>)
{
    ParameterNode expr("da");
    const AllTimeSumNode node(&expr);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&node);
    const auto cloneAllTimeSum = dynamic_cast<AllTimeSumNode*>(clone);

    BOOST_REQUIRE(cloneAllTimeSum);

    const auto expression = dynamic_cast<ParameterNode*>(cloneAllTimeSum->child());
    BOOST_REQUIRE(expression);
    BOOST_CHECK(expression->value() == "da");
}

BOOST_FIXTURE_TEST_CASE(CloneVisitor_name, Registry<Node>)
{
    CloneVisitor cloneVisitor(*this);
    BOOST_CHECK_EQUAL(cloneVisitor.name(), "CloneVisitor");
}

BOOST_AUTO_TEST_SUITE_END()
