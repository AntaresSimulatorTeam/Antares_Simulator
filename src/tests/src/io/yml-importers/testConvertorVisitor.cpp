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
#include <stdexcept>
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/expressions/Registry.hxx"
#include "antares/expressions/visitors/CompareVisitor.h"
#include "antares/io/inputs/model-converter/convertorVisitor.h"
#include "antares/io/inputs/yml-model/Library.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>

#include "antares/io/inputs/model-converter/ForbiddenNodes.h"
#include "antares/io/inputs/model-converter/NodeChecker.h"
// clang-format on

using namespace Antares::Expressions;
using namespace Antares::IO::Inputs;
using namespace Antares::IO::Inputs::ModelConverter;

static Nodes::LiteralNode* toLiteral(Nodes::Node* n)
{
    return dynamic_cast<Nodes::LiteralNode*>(n);
}

BOOST_AUTO_TEST_CASE(empty_expression)
{
    BOOST_CHECK_EQUAL(convertExpressionToNode("", {} /* empty model*/).node, nullptr);
}

BOOST_AUTO_TEST_CASE(negation)
{
    std::string expression = "-7";

    auto expr = convertExpressionToNode(expression, {} /* empty model*/);

    BOOST_CHECK_EQUAL(expr.node->name(), "NegationNode");
    auto* nodeNeg = dynamic_cast<Nodes::NegationNode*>(expr.node);
    BOOST_REQUIRE(nodeNeg);
    BOOST_CHECK_EQUAL(toLiteral(nodeNeg->child())->value(), 7);
}

BOOST_AUTO_TEST_CASE(identifier)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "param1";
    auto expr = convertExpressionToNode(expression, model);

    BOOST_CHECK_EQUAL(expr.node->name(), "ParameterNode");

    expression = "varP";
    expr = convertExpressionToNode(expression, model);
    BOOST_CHECK_EQUAL(expr.node->name(), "VariableNode");
}

BOOST_AUTO_TEST_CASE(identifierNotFound)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}},
      .variables = {{"varP", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "abc"; // not a param or var
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::runtime_error,
                          checkMessage("No parameter or variable found for this identifier: abc"));
}

BOOST_AUTO_TEST_CASE(addTwoLiterals)
{
    const std::string expression = "1 + 2";

    auto expr = convertExpressionToNode(expression, {} /* empty model*/);

    BOOST_CHECK_EQUAL(expr.node->name(), "SumNode");
    auto* nodeSum = dynamic_cast<Nodes::SumNode*>(expr.node);
    BOOST_REQUIRE(nodeSum);
    const auto& operands = nodeSum->getOperands();
    BOOST_CHECK_EQUAL(toLiteral(operands[0])->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(operands[1])->value(), 2);
}

BOOST_AUTO_TEST_CASE(addThreeLiterals)
{
    /*
      Desired behavior
      "1 + 2 + 3" -> SumNode(1,2,3)
    */

    const std::string expression = "1 + 2 + 3";

    auto expr = convertExpressionToNode(expression, {} /* empty model*/);

    auto* nodeSum = dynamic_cast<Nodes::SumNode*>(expr.node);
    BOOST_REQUIRE(nodeSum);
    const auto& operands = nodeSum->getOperands();
    BOOST_REQUIRE_EQUAL(operands.size(), 3);
    BOOST_CHECK_EQUAL(toLiteral(operands[0])->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(operands[1])->value(), 2);
    BOOST_CHECK_EQUAL(toLiteral(operands[2])->value(), 3);
}

BOOST_AUTO_TEST_CASE(subtractTwoLiterals)
{
    const std::string expression = "6 - 3";

    auto expr = convertExpressionToNode(expression, {} /* empty model*/);

    BOOST_CHECK_EQUAL(expr.node->name(), "SubtractionNode");
    auto* nodeSub = dynamic_cast<Nodes::SubtractionNode*>(expr.node);
    BOOST_REQUIRE(nodeSub);
    BOOST_CHECK_EQUAL(toLiteral(nodeSub->left())->value(), 6);
    BOOST_CHECK_EQUAL(toLiteral(nodeSub->right())->value(), 3);
}

BOOST_AUTO_TEST_CASE(multiplyTwoLiterals)
{
    std::string expression = "1 * 2";

    auto expr = convertExpressionToNode(expression, {} /* empty model*/);

    BOOST_CHECK_EQUAL(expr.node->name(), "MultiplicationNode");
    auto* nodeMult = dynamic_cast<Nodes::MultiplicationNode*>(expr.node);
    BOOST_REQUIRE(nodeMult);
    BOOST_CHECK_EQUAL(toLiteral(nodeMult->left())->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(nodeMult->right())->value(), 2);
}

BOOST_AUTO_TEST_CASE(divideTwoLiterals)
{
    const std::string expression = "6 / 3";

    auto expr = convertExpressionToNode(expression, {} /* empty model*/);

    BOOST_CHECK_EQUAL(expr.node->name(), "DivisionNode");
    auto* nodeDiv = dynamic_cast<Nodes::DivisionNode*>(expr.node);
    BOOST_REQUIRE(nodeDiv);
    BOOST_CHECK_EQUAL(toLiteral(nodeDiv->left())->value(), 6);
    BOOST_CHECK_EQUAL(toLiteral(nodeDiv->right())->value(), 3);
}

BOOST_AUTO_TEST_CASE(comparison)
{
    std::string expression = "1 = 2";
    auto expr = convertExpressionToNode(expression, {} /* empty model*/);
    BOOST_CHECK_EQUAL(expr.node->name(), "EqualNode");

    expression = "1 <= 5";
    expr = convertExpressionToNode(expression, {} /* empty model*/);
    BOOST_CHECK_EQUAL(expr.node->name(), "LessThanOrEqualNode");

    expression = "8364 >= 27";
    expr = convertExpressionToNode(expression, {} /* empty model*/);
    BOOST_CHECK_EQUAL(expr.node->name(), "GreaterThanOrEqualNode");

    auto* nodeGreater = dynamic_cast<Nodes::GreaterThanOrEqualNode*>(expr.node);
    BOOST_REQUIRE(nodeGreater);
    BOOST_CHECK_EQUAL(toLiteral(nodeGreater->left())->value(), 8364);
    BOOST_CHECK_EQUAL(toLiteral(nodeGreater->right())->value(), 27);
}

BOOST_AUTO_TEST_CASE(portfield)
{
    YmlModel::Model model{.id = "model0",
                          .description = "description",
                          .parameters = {},
                          .variables = {},
                          .ports = {{.id = "port1", .type = "blue"}},
                          .port_field_definitions = {{"port1", "field1", ""}},
                          .constraints = {},
                          .binding_constraints = {},
                          .objectives = {{"objective-id", ""}},
                          .extra_outputs = {}};
    std::string expression = "port1.field1";
    auto expr = convertExpressionToNode(expression, model);

    BOOST_CHECK_EQUAL(expr.node->name(), "PortFieldNode");

    expression = "port2.field1";
    BOOST_CHECK_THROW(convertExpressionToNode(expression, model), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(portfieldSum)
{
    YmlModel::Model model{.id = "model0",
                          .description = "description",
                          .parameters = {},
                          .variables = {},
                          .ports = {{.id = "port1", .type = "blue"}},
                          .port_field_definitions = {{"port1", "field1", ""}},
                          .constraints = {},
                          .binding_constraints = {},
                          .objectives = {{"objective-id", ""}},
                          .extra_outputs = {}};

    std::string expression = "sum_connections(port1.field1)";
    auto expr = convertExpressionToNode(expression, model);

    BOOST_CHECK_EQUAL(expr.node->name(), "PortFieldSumNode");
    auto portFieldSumNode = dynamic_cast<Nodes::PortFieldSumNode*>(expr.node);
    BOOST_REQUIRE(portFieldSumNode);
    BOOST_CHECK_EQUAL(portFieldSumNode->getPortName(), "port1");
    BOOST_CHECK_EQUAL(portFieldSumNode->getFieldName(), "field1");

    expression = "port2.field1";
    BOOST_CHECK_THROW(convertExpressionToNode(expression, model), std::runtime_error);
}

YmlModel::Model createYmlModel()
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "param1", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    return model;
}

std::pair<std::string, Nodes::Node*> expected_expression(Registry<Nodes::Node>& registry)
{
    auto* param = registry.create<Nodes::ParameterNode>("param1");
    auto* var = registry.create<Nodes::VariableNode>("varP", 89);
    auto* l3 = registry.create<Nodes::LiteralNode>(3);
    auto* l42 = registry.create<Nodes::LiteralNode>(42);
    auto* l1 = registry.create<Nodes::LiteralNode>(1);
    auto* l4 = registry.create<Nodes::LiteralNode>(4);
    auto* l12 = registry.create<Nodes::LiteralNode>(12);
    auto* sub = registry.create<Nodes::SubtractionNode>(l4, l1);
    auto* mult = registry.create<Nodes::MultiplicationNode>(l12, sub);
    auto* sum1 = registry.create<Nodes::SumNode>(mult, param);
    auto* sum2 = registry.create<Nodes::SumNode>(l42, l3, var);
    auto* neg = registry.create<Nodes::NegationNode>(sum2);
    auto* div = registry.create<Nodes::DivisionNode>(sum1, neg);
    return {"(12 * (4 - 1) + param1) / -(42 + 3 + varP)", div};
}

struct RegistryHolder
{
    Registry<Nodes::Node> registry;
};

BOOST_FIXTURE_TEST_CASE(medium_expression, RegistryHolder)
{
    const auto [expression, div] = expected_expression(registry);
    const auto expr = convertExpressionToNode(expression, createYmlModel());

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, div));
}

BOOST_FIXTURE_TEST_CASE(NumericalTimeIndexExpresion, RegistryHolder)
{
    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "(" + expression + ")" + "[12]";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* time_index = registry.create<Nodes::LiteralNode>(12);
    auto* timeIndexNode = registry.create<Nodes::TimeIndexNode>(div, time_index);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeIndexNode));
}

BOOST_FIXTURE_TEST_CASE(TimeIndexExpresion, RegistryHolder)
{
    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "(" + expression + ")" + "[param1]";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* time_index = registry.create<Nodes::ParameterNode>("param1");
    auto* timeIndexNode = registry.create<Nodes::TimeIndexNode>(div, time_index);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeIndexNode));
}

BOOST_FIXTURE_TEST_CASE(TimeShiftExpression, RegistryHolder)
{
    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "(" + expression + ")" + "[t-89]";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* lit = registry.create<Nodes::LiteralNode>(89);
    auto* neg = registry.create<Nodes::NegationNode>(lit);
    auto* timeShiftNode = registry.create<Nodes::TimeShiftNode>(div, neg);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeShiftNode));
}

BOOST_FIXTURE_TEST_CASE(TimeShiftExpressionMul, RegistryHolder)
{
    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "(" + expression + ")" + "[t-89*param1]";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* lit = registry.create<Nodes::LiteralNode>(89);
    auto* neg = registry.create<Nodes::NegationNode>(lit);

    auto* param = registry.create<Nodes::ParameterNode>("param1");
    auto* mult = registry.create<Nodes::MultiplicationNode>(neg, param);
    auto* timeShiftNode = registry.create<Nodes::TimeShiftNode>(div, mult);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeShiftNode));
}

BOOST_FIXTURE_TEST_CASE(TimeSumExpression, RegistryHolder)
{
    const auto [e, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "sum(t-89*param1 .. t+(param1/89)," + e + ")";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* lit = registry.create<Nodes::LiteralNode>(89);
    auto* neg = registry.create<Nodes::NegationNode>(lit);

    auto* param = registry.create<Nodes::ParameterNode>("param1");
    auto* from = registry.create<Nodes::MultiplicationNode>(neg, param);
    auto* to = registry.create<Nodes::DivisionNode>(param, lit);
    const auto* timeSumNode = registry.create<Nodes::TimeSumNode>(from, to, div);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeSumNode));
}

BOOST_FIXTURE_TEST_CASE(AlltimeSumExpression, RegistryHolder)
{
    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "sum(" + expression + ")";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    const auto* timeSumNode = registry.create<Nodes::AllTimeSumNode>(div);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeSumNode));
}

BOOST_AUTO_TEST_CASE(dualExpression)
{
    YmlModel::Model model{.id = "model0",
                          .description = "description",
                          .parameters = {},
                          .variables = {},
                          .ports = {},
                          .port_field_definitions = {},
                          .constraints = {{"constraintA", ""}},
                          .binding_constraints = {{"constraintB", ""}},
                          .objectives = {{"objective-id", ""}},
                          .extra_outputs = {}};

    // constraints
    std::string expression = "dual(constraintA)";
    auto expr = convertExpressionToNode(expression, model);

    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::dual");
    auto dualNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dualNode->typeToString(), "dual");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::ParameterNode*>(dualNode->getOperands().at(0))->value(),
                      "constraintA");

    // binding constraints
    expression = "dual(constraintB)";
    expr = convertExpressionToNode(expression, model);

    dualNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::ParameterNode*>(dualNode->getOperands().at(0))->value(),
                      "constraintB");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::LiteralNode*>(dualNode->getOperands().at(1))->value(), 1);
    std::string badExpression = "dual(abc)";
    std::string expected_msg = "dual called with unknown constraint 'abc' in model 'model0'";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(badExpression, model),
                          std::runtime_error,
                          checkMessage(expected_msg));
}

BOOST_AUTO_TEST_CASE(EmptyDualExpression)
{
    YmlModel::Model model{.id = "model0",
                          .description = "description",
                          .parameters = {},
                          .variables = {},
                          .ports = {},
                          .port_field_definitions = {},
                          .constraints = {{"constraintA", ""}},
                          .binding_constraints = {{"constraintB", ""}},
                          .objectives = {{"objective-id", ""}},
                          .extra_outputs = {}};

    std::string expression = "dual()";
    std::string expected_msg = "dual operator expects an argument, got nothing";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(expected_msg));
}

BOOST_AUTO_TEST_CASE(WrongDualExpression)
{
    YmlModel::Model model{.id = "model0",
                          .description = "description",
                          .parameters = {},
                          .variables = {},
                          .ports = {},
                          .port_field_definitions = {},
                          .constraints = {{"constraintA", ""}},
                          .binding_constraints = {{"constraintB", ""}},
                          .objectives = {{"objective-id", ""}},
                          .extra_outputs = {}};

    std::string expression = "dual(constraintA, e^(iPi) + 1 = 0)";
    auto err_msg = "dual operator expects exactly one constraint id got: constraintA, e^(iPi)+1=0";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(reducedCostExpression)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "reduced_cost(varB)";
    auto expr = convertExpressionToNode(expression, model);
    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::reduced_cost");
    auto reducedCostNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(reducedCostNode->typeToString(), "reduced_cost");
    const auto* variableNode = dynamic_cast<Nodes::VariableNode*>(
      reducedCostNode->getOperands().at(0));
    BOOST_CHECK_EQUAL(variableNode->value(), "varB");
    BOOST_CHECK_EQUAL(variableNode->Index(), 1);

    std::string badExpression = "reduced_cost(abc)";
    std::string err_msg = "reduced_cost called with unknown variable 'abc' in model 'model0'";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(badExpression, model),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(reducedCostExpressionTwoVariables)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "reduced_cost(varB, 2)";
    std::string err_msg = "reduced_cost operator expects exactly one variable id got: varB, 2";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(EmptyReducedCostExpression)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    // constraints
    std::string expression = "reduced_cost()";
    std::string err_msg = "reduced_cost operator expects an argument, got nothing";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(ValidPowerExpression)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "varB^2";
    auto expr = convertExpressionToNode(expression, model);
    auto* powerNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::VariableNode*>(powerNode->getOperands().at(0))->value(),
                      "varB");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::LiteralNode*>(powerNode->getOperands().at(1))->value(),
                      2);
}

BOOST_AUTO_TEST_CASE(EmptyPowerExpression)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "^";
    BOOST_CHECK_THROW(convertExpressionToNode(expression, model), AntlrParsingError);
}

BOOST_AUTO_TEST_CASE(WrongPowerExpression)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "varA^_";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          NoParameterOrVariableWithThisName,
                          checkMessage("No parameter or variable found for this identifier: _"));
}

BOOST_AUTO_TEST_CASE(ValidMinExpression)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "min(varB, 2, pmin)";
    auto expr = convertExpressionToNode(expression, model);
    auto* minNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::VariableNode*>(minNode->getOperands().at(0))->value(),
                      "varB");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::LiteralNode*>(minNode->getOperands().at(1))->value(), 2);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::ParameterNode*>(minNode->getOperands().at(2))->value(),
                      "pmin");
}

BOOST_AUTO_TEST_CASE(ValidMaxExpression)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "min(varB, 2, pmin, varA, varB^pmin)";
    auto expr = convertExpressionToNode(expression, model);

    auto* maxNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::VariableNode*>(maxNode->getOperands().at(0))->value(),
                      "varB");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::LiteralNode*>(maxNode->getOperands().at(1))->value(), 2);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::ParameterNode*>(maxNode->getOperands().at(2))->value(),
                      "pmin");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::VariableNode*>(maxNode->getOperands().at(3))->value(),
                      "varA");
    auto* node5 = dynamic_cast<Nodes::FunctionNode*>(maxNode->getOperands().at(4));
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::VariableNode*>(node5->getOperands().at(0))->value(),
                      "varB");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::ParameterNode*>(node5->getOperands().at(1))->value(),
                      "pmin");
}

BOOST_AUTO_TEST_CASE(MaxOperatorWrongNumberOfParameter)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "max(varB)";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage("max operator expects at least 2 operands got 1"));
}

BOOST_AUTO_TEST_CASE(MinOperatorWrongNumberOfParameter)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "min(varB)";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage("min operator expects at least 2 operands got 1"));
}

BOOST_AUTO_TEST_CASE(MinWithForbiddenNode)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "min(varB, reduced_cost(varB))";

    // forbid variable in min
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addForbiddenFor<Nodes::FunctionNodeType::min, Nodes::VariableNode>();

    auto node = convertExpressionToNode(expression, model);

    auto err_msg = "'min' is not allowed to contain 'variable(varB)' in this context '" + expression
                   + "'";
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(MaxWithForbiddenNode)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "max(reduced_cost(varB), pmin, varA)";
    // forbid variable in max
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addForbiddenFor<Nodes::FunctionNodeType::max, Nodes::VariableNode>();
    auto node = convertExpressionToNode(expression, model);
    std::string err_msg = "'max' is not allowed to contain 'variable(varB)' in this context '"
                          + expression + "'";
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(ExpressionThatNotContainComparisonSignLT)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "varA <= 38";
    // forbid <= Globally
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addGlobalForbidden<Nodes::LessThanOrEqualNode>();
    auto node = convertExpressionToNode(expression, model);
    std::string err_msg = "'expression with <=' is not allowed in this context '" + expression
                          + "'";
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(ExpressionThatNotContainComparisonSignGT)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "varA >= 38";
    // forbid <= Globally
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addGlobalForbidden<Nodes::GreaterThanOrEqualNode>();
    auto node = convertExpressionToNode(expression, model);
    std::string err_msg = "'expression with >=' is not allowed in this context '" + expression
                          + "'";
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(ExpressionThatNotContainEqualSign)
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false},
                    {"varB", "7", "pmin", YmlModel::ValueType::CONTINUOUS, false, false}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", ""}},
      .extra_outputs = {}};

    std::string expression = "varA = 38";
    // forbid <= Globally
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addGlobalForbidden<Nodes::EqualNode>();
    auto node = convertExpressionToNode(expression, model);
    std::string err_msg = "'expression with =' is not allowed in this context '" + expression + "'";
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage(err_msg));
}
