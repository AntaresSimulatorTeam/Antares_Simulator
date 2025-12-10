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

#include <any>

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

class ExpressionToNodeConvertorEmptyModel
{
public:
    ExpressionToNodeConvertorEmptyModel(YmlModel::Model&& model):
        model_(std::move(model))
    {
    }

    // Empty model
    ExpressionToNodeConvertorEmptyModel() = default;

    NodeRegistry run(const std::string& input)
    {
        return ModelConverter::convertExpressionToNode(input, model_);
    }

private:
    const YmlModel::Model model_;
};

static Nodes::LiteralNode* toLiteral(Nodes::Node* n)
{
    return dynamic_cast<Nodes::LiteralNode*>(n);
}

BOOST_FIXTURE_TEST_CASE(empty_expression, ExpressionToNodeConvertorEmptyModel)
{
    BOOST_CHECK_EQUAL(run("").node, nullptr);
}

BOOST_FIXTURE_TEST_CASE(negation, ExpressionToNodeConvertorEmptyModel)
{
    std::string expression = "-7";
    auto expr = run(expression);
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    {
        std::string expression = "param1";
        auto expr = converter.run(expression);
        BOOST_CHECK_EQUAL(expr.node->name(), "ParameterNode");
    }

    {
        std::string expression = "varP";
        auto expr = converter.run(expression);
        BOOST_CHECK_EQUAL(expr.node->name(), "VariableNode");
    }
}

bool expectedMessage(const std::runtime_error& ex)
{
    BOOST_CHECK_EQUAL(ex.what(),
                      std::string("No parameter or variable found for this identifier: abc"));
    return true;
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
    BOOST_CHECK_EXCEPTION(ModelConverter::convertExpressionToNode(expression, model),
                          std::runtime_error,
                          expectedMessage);
}

BOOST_FIXTURE_TEST_CASE(addTwoLiterals, ExpressionToNodeConvertorEmptyModel)
{
    const std::string expression = "1 + 2";
    auto expr = run(expression);
    BOOST_CHECK_EQUAL(expr.node->name(), "SumNode");

    auto* nodeSum = dynamic_cast<Nodes::SumNode*>(expr.node);
    BOOST_REQUIRE(nodeSum);
    const auto& operands = nodeSum->getOperands();
    BOOST_CHECK_EQUAL(toLiteral(operands[0])->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(operands[1])->value(), 2);
}

BOOST_FIXTURE_TEST_CASE(addThreeLiterals, ExpressionToNodeConvertorEmptyModel)
{
    /*
      Desired behavior
      "1+2+3" -> SumNode(1,2,3)
    */

    const std::string expression = "1 + 2 + 3";
    auto expr = run(expression);

    auto* nodeSum = dynamic_cast<Nodes::SumNode*>(expr.node);
    BOOST_REQUIRE(nodeSum);
    const auto& operands = nodeSum->getOperands();
    BOOST_REQUIRE_EQUAL(operands.size(), 3);
    BOOST_CHECK_EQUAL(toLiteral(operands[0])->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(operands[1])->value(), 2);
    BOOST_CHECK_EQUAL(toLiteral(operands[2])->value(), 3);
}

BOOST_FIXTURE_TEST_CASE(subtractTwoLiterals, ExpressionToNodeConvertorEmptyModel)
{
    const std::string expression = "6 - 3";
    auto expr = run(expression);
    BOOST_CHECK_EQUAL(expr.node->name(), "SubtractionNode");

    auto* nodeSub = dynamic_cast<Nodes::SubtractionNode*>(expr.node);
    BOOST_REQUIRE(nodeSub);
    BOOST_CHECK_EQUAL(toLiteral(nodeSub->left())->value(), 6);
    BOOST_CHECK_EQUAL(toLiteral(nodeSub->right())->value(), 3);
}

BOOST_FIXTURE_TEST_CASE(multiplyTwoLiterals, ExpressionToNodeConvertorEmptyModel)
{
    std::string expression = "1 * 2";
    auto expr = run(expression);
    BOOST_CHECK_EQUAL(expr.node->name(), "MultiplicationNode");

    auto* nodeMult = dynamic_cast<Nodes::MultiplicationNode*>(expr.node);
    BOOST_REQUIRE(nodeMult);
    BOOST_CHECK_EQUAL(toLiteral(nodeMult->left())->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(nodeMult->right())->value(), 2);
}

BOOST_FIXTURE_TEST_CASE(divideTwoLiterals, ExpressionToNodeConvertorEmptyModel)
{
    const std::string expression = "6 / 3";
    auto expr = run(expression);
    BOOST_CHECK_EQUAL(expr.node->name(), "DivisionNode");

    auto* nodeDiv = dynamic_cast<Nodes::DivisionNode*>(expr.node);
    BOOST_REQUIRE(nodeDiv);
    BOOST_CHECK_EQUAL(toLiteral(nodeDiv->left())->value(), 6);
    BOOST_CHECK_EQUAL(toLiteral(nodeDiv->right())->value(), 3);
}

BOOST_FIXTURE_TEST_CASE(comparison, ExpressionToNodeConvertorEmptyModel)
{
    std::string expression = "1 = 2";
    auto expr = run(expression);
    BOOST_CHECK_EQUAL(expr.node->name(), "EqualNode");

    expression = "1 <= 5";
    expr = run(expression);
    BOOST_CHECK_EQUAL(expr.node->name(), "LessThanOrEqualNode");

    expression = "8364 >= 27";
    expr = run(expression);
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

    ExpressionToNodeConvertorEmptyModel converter(std::move(model));
    std::string expression = "port1.field1";
    auto expr = converter.run(expression);

    BOOST_CHECK_EQUAL(expr.node->name(), "PortFieldNode");

    expression = "port2.field1";
    BOOST_CHECK_THROW(converter.run(expression), std::runtime_error);
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

    ExpressionToNodeConvertorEmptyModel converter(std::move(model));
    std::string expression = "sum_connections(port1.field1)";
    auto expr = converter.run(expression);

    BOOST_CHECK_EQUAL(expr.node->name(), "PortFieldSumNode");
    auto portFieldSumNode = dynamic_cast<Nodes::PortFieldSumNode*>(expr.node);
    BOOST_REQUIRE(portFieldSumNode);
    BOOST_CHECK_EQUAL(portFieldSumNode->getPortName(), "port1");
    BOOST_CHECK_EQUAL(portFieldSumNode->getFieldName(), "field1");
    expression = "port2.field1";
    BOOST_CHECK_THROW(converter.run(expression), std::runtime_error);
}

ExpressionToNodeConvertorEmptyModel createMediumExpression()
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

    return {std::move(model)};
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

BOOST_AUTO_TEST_CASE(medium_expression)
{
    Registry<Nodes::Node> registry;

    const auto [expression, div] = expected_expression(registry);
    const auto expr = createMediumExpression().run(expression);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, div));
}

BOOST_AUTO_TEST_CASE(NumericalTimeIndexExpresion)
{
    Registry<Nodes::Node> registry;

    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithNumericalTimeIndex = "(" + expression + ")" + "[12]";
    auto expr = createMediumExpression().run(expressionWithNumericalTimeIndex);

    auto* time_index = registry.create<Nodes::LiteralNode>(12);
    auto* timeIndexNode = registry.create<Nodes::TimeIndexNode>(div, time_index);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeIndexNode));
}

BOOST_AUTO_TEST_CASE(TimeIndexExpresion)
{
    Registry<Nodes::Node> registry;

    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithNumericalTimeIndex = "(" + expression + ")" + "[param1]";
    auto expr = createMediumExpression().run(expressionWithNumericalTimeIndex);

    auto* time_index = registry.create<Nodes::ParameterNode>("param1");
    auto* timeIndexNode = registry.create<Nodes::TimeIndexNode>(div, time_index);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeIndexNode));
}

BOOST_AUTO_TEST_CASE(TimeShiftExpression)
{
    Registry<Nodes::Node> registry;

    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithNumericalTimeIndex = "(" + expression + ")" + "[t-89]";
    auto expr = createMediumExpression().run(expressionWithNumericalTimeIndex);

    auto* lit = registry.create<Nodes::LiteralNode>(89);
    auto* neg = registry.create<Nodes::NegationNode>(lit);
    auto* timeShiftNode = registry.create<Nodes::TimeShiftNode>(div, neg);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeShiftNode));
}

BOOST_AUTO_TEST_CASE(TimeShiftExpressionMul)
{
    Registry<Nodes::Node> registry;

    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithNumericalTimeIndex = "(" + expression + ")" + "[t-89*param1]";
    auto expr = createMediumExpression().run(expressionWithNumericalTimeIndex);

    auto* lit = registry.create<Nodes::LiteralNode>(89);
    auto* neg = registry.create<Nodes::NegationNode>(lit);

    auto* param = registry.create<Nodes::ParameterNode>("param1");
    auto* mult = registry.create<Nodes::MultiplicationNode>(neg, param);
    auto* timeShiftNode = registry.create<Nodes::TimeShiftNode>(div, mult);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeShiftNode));
}

BOOST_AUTO_TEST_CASE(TimeSumExpression)
{
    Registry<Nodes::Node> registry;

    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithNumericalTimeIndex = "sum(t-89*param1 .. t+(param1/89)," + expression
                                                  + ")";
    auto expr = createMediumExpression().run(expressionWithNumericalTimeIndex);

    auto* lit = registry.create<Nodes::LiteralNode>(89);
    auto* neg = registry.create<Nodes::NegationNode>(lit);

    auto* param = registry.create<Nodes::ParameterNode>("param1");
    auto* from = registry.create<Nodes::MultiplicationNode>(neg, param);
    auto* to = registry.create<Nodes::DivisionNode>(param, lit);
    const auto* timeSumNode = registry.create<Nodes::TimeSumNode>(from, to, div);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeSumNode));
}

BOOST_AUTO_TEST_CASE(AlltimeSumExpression)
{
    Registry<Nodes::Node> registry;

    const auto [expression, div] = expected_expression(registry);
    const auto expressionWithNumericalTimeIndex = "sum(" + expression + ")";
    auto expr = createMediumExpression().run(expressionWithNumericalTimeIndex);

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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    // constraints
    std::string expression = "dual(constraintA)";
    auto expr = converter.run(expression);
    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::dual");
    auto dualNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dualNode->typeToString(), "dual");

    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::ParameterNode*>(dualNode->getOperands().at(0))->value(),
                      "constraintA");

    // binding constraints
    expression = "dual(constraintB)";
    expr = converter.run(expression);
    dualNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::ParameterNode*>(dualNode->getOperands().at(0))->value(),
                      "constraintB");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::LiteralNode*>(dualNode->getOperands().at(1))->value(), 1);

    std::string badExpression = "dual(abc)";
    BOOST_CHECK_EXCEPTION(converter.run(badExpression),
                          std::runtime_error,
                          checkMessage(
                            "dual called with unknown constraint 'abc' in model 'model0'"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    // constraints
    std::string expression = "dual()";
    BOOST_CHECK_EXCEPTION(converter.run(expression),
                          std::invalid_argument,
                          checkMessage(
                            "dual operator expect exactly one constraint id got nothing"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    // constraints
    std::string expression = "dual(constraintA, e^(iPi) + 1 = 0)";
    BOOST_CHECK_EXCEPTION(
      converter.run(expression),
      std::invalid_argument,
      checkMessage("dual operator expect exactly one constraint id got: constraintA, e^(iPi)+1=0"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "reduced_cost(varB)";
    auto expr = converter.run(expression);
    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::reduced_cost");
    auto reducedCostNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(reducedCostNode->typeToString(), "reduced_cost");
    const auto* variableNode = dynamic_cast<Nodes::VariableNode*>(
      reducedCostNode->getOperands().at(0));
    BOOST_CHECK_EQUAL(variableNode->value(), "varB");
    BOOST_CHECK_EQUAL(variableNode->Index(), 1);

    std::string badExpression = "reduced_cost(abc)";
    BOOST_CHECK_EXCEPTION(converter.run(badExpression),
                          std::runtime_error,
                          checkMessage(
                            "reduced_cost called with unknown variable 'abc' in model 'model0'"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "reduced_cost(varB, 2)";
    BOOST_CHECK_EXCEPTION(converter.run(expression),
                          std::invalid_argument,
                          checkMessage(
                            "reduced_cost operator expect exactly one variable id got: varB, 2"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    // constraints
    std::string expression = "reduced_cost()";
    BOOST_CHECK_EXCEPTION(converter.run(expression),
                          std::invalid_argument,
                          checkMessage(
                            "reduced_cost operator expect exactly one variable id got nothing"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "varB^2";
    auto expr = converter.run(expression);
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "^";
    BOOST_CHECK_THROW(converter.run(expression), AntlrParsingError);
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "varA^_";
    BOOST_CHECK_EXCEPTION(converter.run(expression),
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "min(varB, 2, pmin)";
    auto expr = converter.run(expression);
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "min(varB, 2, pmin, varA, varB^pmin)";
    auto expr = converter.run(expression);
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "max(varB)";
    BOOST_CHECK_EXCEPTION(converter.run(expression),
                          std::invalid_argument,
                          checkMessage("max operator expect at least 2 operands got 1"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "min(varB)";
    BOOST_CHECK_EXCEPTION(converter.run(expression),
                          std::invalid_argument,
                          checkMessage("min operator expect at least 2 operands got 1"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "min(varB, reduced_cost(varB))";

    // forbid variable in min
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addForbiddenFor<Nodes::FunctionNodeType::min, Nodes::VariableNode>();
    auto node = converter.run(expression);
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage(
                            "'min' is not allowed to contain 'variable(varB)' in this context '"
                            + expression + "'"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "max(reduced_cost(varB), pmin, varA)";
    // forbid variable in max
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addForbiddenFor<Nodes::FunctionNodeType::max, Nodes::VariableNode>();
    auto node = converter.run(expression);
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage(
                            "'max' is not allowed to contain 'variable(varB)' in this context '"
                            + expression + "'"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "varA <= 38";
    // forbid <= Globally
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addGlobalForbidden<Nodes::LessThanOrEqualNode>();
    auto node = converter.run(expression);
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage("'expression with <=' is not allowed in this context '"
                                       + expression + "'"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "varA >= 38";
    // forbid <= Globally
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addGlobalForbidden<Nodes::GreaterThanOrEqualNode>();
    auto node = converter.run(expression);
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage("'expression with >=' is not allowed in this context '"
                                       + expression + "'"));
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
    ExpressionToNodeConvertorEmptyModel converter(std::move(model));

    std::string expression = "varA = 38";
    // forbid <= Globally
    ModelConverter::ForbiddenNodes forbidden;
    forbidden.addGlobalForbidden<Nodes::EqualNode>();
    auto node = converter.run(expression);
    BOOST_CHECK_EXCEPTION(ModelConverter::NodeChecker(forbidden, expression).dispatch(node.node),
                          ModelConverter::BadContextComposition,
                          checkMessage("'expression with =' is not allowed in this context '"
                                       + expression + "'"));
}
