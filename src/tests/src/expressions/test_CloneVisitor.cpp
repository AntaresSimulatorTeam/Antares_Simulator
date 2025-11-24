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
#include <antares/expressions/visitors/CloneVisitor.h>
#include <antares/expressions/visitors/PrintVisitor.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

BOOST_AUTO_TEST_SUITE(_CloneVisitor_)

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

BOOST_FIXTURE_TEST_CASE(clone_FunctionNodeDual, Registry<Node>)
{
    auto* constraintId = create<ParameterNode>("da");
    auto* constraintIndex = create<LiteralNode>(1459);
    FunctionNode expr(FunctionNodeType::dual, constraintId, constraintIndex);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&expr);
    const auto cloneFunctionNodeDual = dynamic_cast<FunctionNode*>(clone);

    BOOST_REQUIRE(cloneFunctionNodeDual);
    BOOST_CHECK(cloneFunctionNodeDual->typeToString() == "dual");

    const auto& operands = cloneFunctionNodeDual->getOperands();
    const auto cloneConstraintId = dynamic_cast<ParameterNode*>(operands.at(0));
    BOOST_REQUIRE(cloneConstraintId);
    BOOST_CHECK(cloneConstraintId->value() == "da");
    const auto cloneConstraintIndex = dynamic_cast<LiteralNode*>(operands.at(1));
    BOOST_REQUIRE(cloneConstraintIndex);
    BOOST_CHECK(cloneConstraintIndex->value() == 1459);
}

BOOST_FIXTURE_TEST_CASE(clone_FunctionNodeReducedCost, Registry<Node>)
{
    auto* varNode = create<VariableNode>("da", 1501);
    FunctionNode expr(FunctionNodeType::reduced_cost, varNode);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&expr);
    const auto cloneFunctionReducedCost = dynamic_cast<FunctionNode*>(clone);

    BOOST_REQUIRE(cloneFunctionReducedCost);
    BOOST_CHECK(cloneFunctionReducedCost->typeToString() == "reduced_cost");

    const auto& operands = cloneFunctionReducedCost->getOperands();
    const auto cloneVarNode = dynamic_cast<VariableNode*>(operands.at(0));
    BOOST_REQUIRE(cloneVarNode);
    BOOST_CHECK(cloneVarNode->value() == "da");
}

BOOST_FIXTURE_TEST_CASE(clone_FunctionNodePow, Registry<Node>)
{
    auto* base = create<VariableNode>("da", 1501);
    auto* exponent = create<LiteralNode>(1503);
    FunctionNode expr(FunctionNodeType::pow, base, exponent);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&expr);
    const auto cloneFunctionPow = dynamic_cast<FunctionNode*>(clone);

    BOOST_REQUIRE(cloneFunctionPow);
    BOOST_CHECK(cloneFunctionPow->typeToString() == "pow");

    const auto& operands = cloneFunctionPow->getOperands();
    const auto cloneBase = dynamic_cast<VariableNode*>(operands.at(0));
    BOOST_REQUIRE(cloneBase);
    BOOST_CHECK(cloneBase->value() == "da");

    const auto cloneExponent = dynamic_cast<LiteralNode*>(operands.at(1));
    BOOST_REQUIRE(cloneExponent);
    BOOST_CHECK(cloneExponent->value() == 1503);
}

BOOST_FIXTURE_TEST_CASE(clone_FunctionNodeMax, Registry<Node>)
{
    auto* base = create<VariableNode>("da", 1501);
    auto* exponent = create<LiteralNode>(1503);
    FunctionNode expr(FunctionNodeType::max, base, exponent);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&expr);
    const auto cloneFunctionMax = dynamic_cast<FunctionNode*>(clone);

    BOOST_REQUIRE(cloneFunctionMax);
    BOOST_CHECK(cloneFunctionMax->typeToString() == "max");

    const auto& operands = cloneFunctionMax->getOperands();
    const auto cloneBase = dynamic_cast<VariableNode*>(operands.at(0));
    BOOST_REQUIRE(cloneBase);
    BOOST_CHECK(cloneBase->value() == "da");

    const auto cloneExponent = dynamic_cast<LiteralNode*>(operands.at(1));
    BOOST_REQUIRE(cloneExponent);
    BOOST_CHECK(cloneExponent->value() == 1503);
}

BOOST_FIXTURE_TEST_CASE(clone_FunctionNodeMin, Registry<Node>)
{
    auto* base = create<VariableNode>("da", 1501);
    auto* exponent = create<LiteralNode>(1503);
    FunctionNode expr(FunctionNodeType::min, base, exponent);

    CloneVisitor clone_visitor(*this);
    const auto clone = clone_visitor.dispatch(&expr);
    const auto cloneFunctionMin = dynamic_cast<FunctionNode*>(clone);

    BOOST_REQUIRE(cloneFunctionMin);
    BOOST_CHECK(cloneFunctionMin->typeToString() == "min");

    const auto& operands = cloneFunctionMin->getOperands();
    const auto cloneBase = dynamic_cast<VariableNode*>(operands.at(0));
    BOOST_REQUIRE(cloneBase);
    BOOST_CHECK(cloneBase->value() == "da");

    const auto cloneExponent = dynamic_cast<LiteralNode*>(operands.at(1));
    BOOST_REQUIRE(cloneExponent);
    BOOST_CHECK(cloneExponent->value() == 1503);
}

BOOST_FIXTURE_TEST_CASE(CloneVisitor_name, Registry<Node>)
{
    CloneVisitor cloneVisitor(*this);
    BOOST_CHECK_EQUAL(cloneVisitor.name(), "CloneVisitor");
}

BOOST_AUTO_TEST_SUITE_END()
