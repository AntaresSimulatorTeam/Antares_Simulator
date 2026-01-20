// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
#include "antares/io/inputs/model-converter/ForbiddenNodesVisitor.h"
// clang-format on

using namespace Antares::Expressions;
using namespace Antares::IO::Inputs;
using namespace Antares::IO::Inputs::ModelConverter;

static Nodes::LiteralNode* toLiteral(Nodes::Node* n)
{
    return dynamic_cast<Nodes::LiteralNode*>(n);
}

BOOST_AUTO_TEST_SUITE(convertor_visitor)

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

struct SupplyModelForDualOperator
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
};

BOOST_FIXTURE_TEST_CASE(dualExpression, SupplyModelForDualOperator)
{
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

BOOST_FIXTURE_TEST_CASE(EmptyDualExpression, SupplyModelForDualOperator)
{
    std::string expression = "dual()";
    std::string expected_msg = "dual operator expects an argument, got nothing";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(expected_msg));
}

BOOST_FIXTURE_TEST_CASE(WrongDualExpression, SupplyModelForDualOperator)
{
    std::string expression = "dual(constraintA, e^(iPi) + 1 = 0)";
    auto err_msg = "dual operator expects exactly one constraint id got: constraintA, e^(iPi)+1=0";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

struct SupplyModelForFunctionalOperator
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

    ForbiddenNodes forbiddenNodes;
};

BOOST_FIXTURE_TEST_CASE(reducedCostExpression, SupplyModelForFunctionalOperator)
{
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

BOOST_FIXTURE_TEST_CASE(reducedCostExpressionTwoVariables, SupplyModelForFunctionalOperator)
{
    std::string expression = "reduced_cost(varB, 2)";
    std::string err_msg = "reduced_cost operator expects exactly one variable id got: varB, 2";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(EmptyReducedCostExpression, SupplyModelForFunctionalOperator)
{
    std::string expression = "reduced_cost()";
    std::string err_msg = "reduced_cost operator expects an argument, got nothing";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(ValidPowerExpression, SupplyModelForFunctionalOperator)
{
    std::string expression = "varB^2";

    auto expr = convertExpressionToNode(expression, model);

    auto* powerNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::VariableNode*>(powerNode->getOperands().at(0))->value(),
                      "varB");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::LiteralNode*>(powerNode->getOperands().at(1))->value(),
                      2);
}

BOOST_FIXTURE_TEST_CASE(EmptyPowerExpression, SupplyModelForFunctionalOperator)
{
    std::string expression = "^";
    BOOST_CHECK_THROW(convertExpressionToNode(expression, model), AntlrParsingError);
}

BOOST_FIXTURE_TEST_CASE(WrongPowerExpression, SupplyModelForFunctionalOperator)
{
    std::string expression = "varA^_";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          NoParameterOrVariableWithThisName,
                          checkMessage("No parameter or variable found for this identifier: _"));
}

BOOST_FIXTURE_TEST_CASE(ValidMinExpression, SupplyModelForFunctionalOperator)
{
    std::string expression = "min(varB, 2, pmin)";

    auto expr = convertExpressionToNode(expression, model);

    auto* minNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::VariableNode*>(minNode->getOperands().at(0))->value(),
                      "varB");
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::LiteralNode*>(minNode->getOperands().at(1))->value(), 2);
    BOOST_CHECK_EQUAL(dynamic_cast<Nodes::ParameterNode*>(minNode->getOperands().at(2))->value(),
                      "pmin");
}

BOOST_FIXTURE_TEST_CASE(ValidMaxExpression, SupplyModelForFunctionalOperator)
{
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

BOOST_FIXTURE_TEST_CASE(MaxOperatorWrongNumberOfParameter, SupplyModelForFunctionalOperator)
{
    std::string expression = "max(varB)";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage("max operator expects at least 2 operands got 1"));
}

BOOST_FIXTURE_TEST_CASE(MinOperatorWrongNumberOfParameter, SupplyModelForFunctionalOperator)
{
    std::string expression = "min(varB)";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage("min operator expects at least 2 operands got 1"));
}

BOOST_FIXTURE_TEST_CASE(MinWithForbiddenNode, SupplyModelForFunctionalOperator)
{
    std::string expression = "min(varB, reduced_cost(varB))";

    auto node = convertExpressionToNode(expression, model);

    // Forbid variable in min
    forbiddenNodes.parentForbidsChild<Nodes::FunctionNodeType::min, Nodes::VariableNode>();

    auto err_msg = "'FunctionNode::min' is not allowed to contain 'VariableNode' in expression '"
                   + expression + "'";
    BOOST_CHECK_EXCEPTION(ForbiddenNodesVisitor(forbiddenNodes, expression).dispatch(node.node),
                          ForbiddenNodeFound,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(MaxWithForbiddenNode, SupplyModelForFunctionalOperator)
{
    std::string expression = "max(reduced_cost(varB), pmin, varA)";

    auto node = convertExpressionToNode(expression, model);

    // Forbid variable in max
    forbiddenNodes.parentForbidsChild<Nodes::FunctionNodeType::max, Nodes::VariableNode>();

    std::string err_msg = "'FunctionNode::max' is not allowed to contain 'VariableNode' in ";
    err_msg += "expression '" + expression + "'";
    BOOST_CHECK_EXCEPTION(ForbiddenNodesVisitor(forbiddenNodes, expression).dispatch(node.node),
                          ForbiddenNodeFound,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(ExpressionThatNotContainComparisonSignLT, SupplyModelForFunctionalOperator)
{
    std::string expression = "varA <= 38";

    auto node = convertExpressionToNode(expression, model);

    forbiddenNodes.forbidGlobally<Nodes::LessThanOrEqualNode>();

    std::string err_msg = "'LessThanOrEqualNode' is not allowed in expression '" + expression + "'";
    BOOST_CHECK_EXCEPTION(ForbiddenNodesVisitor(forbiddenNodes, expression).dispatch(node.node),
                          ForbiddenNodeFound,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(ExpressionThatNotContainComparisonSignGT, SupplyModelForFunctionalOperator)
{
    std::string expression = "varA >= 38";

    auto node = convertExpressionToNode(expression, model);

    forbiddenNodes.forbidGlobally<Nodes::GreaterThanOrEqualNode>();

    std::string err_msg = "'GreaterThanOrEqualNode' is not allowed in expression '" + expression
                          + "'";
    BOOST_CHECK_EXCEPTION(ForbiddenNodesVisitor(forbiddenNodes, expression).dispatch(node.node),
                          ForbiddenNodeFound,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(ExpressionThatNotContainEqualSign, SupplyModelForFunctionalOperator)
{
    std::string expression = "varA = 38";

    auto node = convertExpressionToNode(expression, model);

    forbiddenNodes.forbidGlobally<Nodes::EqualNode>();

    std::string err_msg = "'EqualNode' is not allowed in expression '" + expression + "'";
    BOOST_CHECK_EXCEPTION(ForbiddenNodesVisitor(forbiddenNodes, expression).dispatch(node.node),
                          ForbiddenNodeFound,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(floor_operator_should_not_take_a_variable_as_arg,
                        SupplyModelForFunctionalOperator)
{
    std::string expression = "floor(varA)";
    auto node = convertExpressionToNode(expression, model);
    forbiddenNodes.parentForbidsChild<Nodes::FunctionNodeType::floor, Nodes::VariableNode>();

    std::string err_msg = "'FunctionNode::floor' is not allowed to contain 'VariableNode' in ";
    err_msg += "expression '" + expression + "'";
    BOOST_CHECK_EXCEPTION(ForbiddenNodesVisitor(forbiddenNodes, expression).dispatch(node.node),
                          ForbiddenNodeFound,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(floor_operator___nominal_case, SupplyModelForFunctionalOperator)
{
    std::string expression = "floor(pmin)";

    auto expr = convertExpressionToNode(expression, model);

    // Root node is a 'floor' node
    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::floor");

    auto FloorNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(FloorNode->typeToString(), "floor");

    // Child node must be unique, and be the parameter node 'pmin' from the model
    // ... Child node is unique
    BOOST_CHECK_EQUAL(FloorNode->getOperands().size(), 1);
    auto* childNode = FloorNode->getOperands()[0];

    // ... Child node is a parameter node
    const auto* paramNode = dynamic_cast<Nodes::ParameterNode*>(childNode);
    BOOST_CHECK(paramNode);

    // ... Child node is 'pmin' from the model
    BOOST_CHECK_EQUAL(paramNode->value(), "pmin");
}

BOOST_FIXTURE_TEST_CASE(floor_operator_applied_to_a_literal, SupplyModelForFunctionalOperator)
{
    std::string expression = "floor(3.7)";

    auto expr = convertExpressionToNode(expression, model);

    // Root node is a 'floor' node
    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::floor");

    auto FloorNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(FloorNode->typeToString(), "floor");

    // Child node must be unique, and be the literal node '3.7' from the model
    // ... Child node is unique
    BOOST_CHECK_EQUAL(FloorNode->getOperands().size(), 1);
    auto* childNode = FloorNode->getOperands()[0];

    // ... Child node is a parameter node
    const auto* literalNode = dynamic_cast<Nodes::LiteralNode*>(childNode);
    BOOST_CHECK(literalNode);

    // ... Child node is 'pmin' from the model
    BOOST_CHECK_EQUAL(literalNode->value(), 3.7);
}

BOOST_FIXTURE_TEST_CASE(floor_operator_should_not_take_no_arg, SupplyModelForFunctionalOperator)
{
    std::string expression = "floor()";

    std::string err_msg = "floor operator expects an argument, got nothing";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(floor_operator_should_not_take_more_than_one_arg,
                        SupplyModelForFunctionalOperator)
{
    std::string expression = "floor(pmin, 5)";

    std::string err_msg = "floor() expects 1 argument, but has 2";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(ceil_operator___nominal_case, SupplyModelForFunctionalOperator)
{
    std::string expression = "ceil(pmin)";

    auto expr = convertExpressionToNode(expression, model);

    // Root node is a 'ceil' node
    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::ceil");

    auto ceilNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(ceilNode->typeToString(), "ceil");

    // Child node must be unique, and be the parameter node 'pmin' from the model
    BOOST_CHECK_EQUAL(ceilNode->getOperands().size(), 1);
    auto* childNode = ceilNode->getOperands()[0];

    const auto* paramNode = dynamic_cast<Nodes::ParameterNode*>(childNode);
    BOOST_CHECK(paramNode);
    BOOST_CHECK_EQUAL(paramNode->value(), "pmin");
}

BOOST_FIXTURE_TEST_CASE(ceil_operator_applied_to_a_literal, SupplyModelForFunctionalOperator)
{
    std::string expression = "ceil(3.7)";

    auto expr = convertExpressionToNode(expression, model);

    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::ceil");

    auto ceilNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_CHECK_EQUAL(ceilNode->typeToString(), "ceil");

    BOOST_CHECK_EQUAL(ceilNode->getOperands().size(), 1);
    auto* childNode = ceilNode->getOperands()[0];

    const auto* literalNode = dynamic_cast<Nodes::LiteralNode*>(childNode);
    BOOST_CHECK(literalNode);
    BOOST_CHECK_EQUAL(literalNode->value(), 3.7);
}

BOOST_FIXTURE_TEST_CASE(ceil_operator_should_not_take_no_arg, SupplyModelForFunctionalOperator)
{
    std::string expression = "ceil()";

    std::string err_msg = "ceil operator expects an argument, got nothing";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(ceil_operator_should_not_take_more_than_one_arg,
                        SupplyModelForFunctionalOperator)
{
    std::string expression = "ceil(pmin, 5)";

    std::string err_msg = "ceil() expects 1 argument, but has 2";
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(ceil_operator_forbidden_on_variable_with_forbidden_nodes,
                        SupplyModelForFunctionalOperator)
{
    std::string expression = "ceil(varA)";

    auto node = convertExpressionToNode(expression, model);

    // Forbid variables as children of ceil
    forbiddenNodes.parentForbidsChild<Nodes::FunctionNodeType::ceil, Nodes::VariableNode>();

    std::string err_msg = "'FunctionNode::ceil' is not allowed to contain 'VariableNode' in "
                          "expression '"
                          + expression + "'";
    BOOST_CHECK_EXCEPTION(ForbiddenNodesVisitor(forbiddenNodes, expression).dispatch(node.node),
                          ForbiddenNodeFound,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()
