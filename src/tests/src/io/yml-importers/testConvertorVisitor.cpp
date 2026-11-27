// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <functional>

#include <boost/test/data/test_case.hpp>
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

#include "antares/io/inputs/forbidden-nodes/ForbiddenNodes.h"
#include "antares/io/inputs/forbidden-nodes/ForbiddenNodesVisitor.h"
// clang-format on

using namespace Antares::Expressions;
using namespace Antares::IO::Inputs;
using namespace Antares::IO::Inputs::ForbidNodes;
using namespace Antares::IO::Inputs::ModelConverter;

static Nodes::LiteralNode* toLiteral(Nodes::Node* n)
{
    return dynamic_cast<Nodes::LiteralNode*>(n);
}

YmlModel::Model createYmlModel()
{
    return {.id = "model0",
            .description = "description",
            .parameters = {{"param1", true, false}, {"param2", false, false}},
            .variables = {{"varP",
                           YmlModel::ExpressionLineNumber{"7", 0},
                           YmlModel::ExpressionLineNumber{"pmin", 0},
                           YmlModel::ValueType::CONTINUOUS,
                           false,
                           false,
                           "test.yaml"}},
            .ports = {},
            .port_field_definitions = {},
            .constraints = {},
            .binding_constraints = {},
            .objectives = {{"objective-id", YmlModel::ExpressionLineNumber{"", 0}, "test.yaml"}},
            .extra_outputs = {},
            .filename = ""};
}

YmlModel::Model createModelWithPortField()
{
    return {.id = "model0",
            .description = "description",
            .parameters = {},
            .variables = {},
            .ports = {{.id = "port1", .type = "blue"}},
            .port_field_definitions = {{"port1", "field1", YmlModel::ExpressionLineNumber{"", 0}}},
            .constraints = {},
            .binding_constraints = {},
            .objectives = {{"objective-id", YmlModel::ExpressionLineNumber{"", 0}, "test.yaml"}},
            .extra_outputs = {},
            .filename = ""};
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
    const auto model = createYmlModel();

    std::string expression = "param1";
    auto expr = convertExpressionToNode(expression, model);

    BOOST_CHECK_EQUAL(expr.node->name(), "ParameterNode");

    expression = "varP";
    expr = convertExpressionToNode(expression, model);
    BOOST_CHECK_EQUAL(expr.node->name(), "VariableNode");
}

BOOST_AUTO_TEST_CASE(identifierNotFound)
{
    const auto model = createYmlModel();

    std::string expression = "abc"; // not a param or var
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model),
                          InputError,
                          containsMessage(
                            "No parameter or variable found for this identifier: abc"));
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
    const auto model = createModelWithPortField();
    std::string expression = "port1.field1";
    auto expr = convertExpressionToNode(expression, model);

    BOOST_CHECK_EQUAL(expr.node->name(), "PortFieldNode");

    expression = "port2.field1";
    BOOST_CHECK_THROW(convertExpressionToNode(expression, model), InputError);
}

BOOST_AUTO_TEST_CASE(portfieldSum)
{
    const auto model = createModelWithPortField();

    std::string expression = "sum_connections(port1.field1)";
    auto expr = convertExpressionToNode(expression, model);

    BOOST_CHECK_EQUAL(expr.node->name(), "PortFieldSumNode");
    auto portFieldSumNode = dynamic_cast<Nodes::PortFieldSumNode*>(expr.node);
    BOOST_REQUIRE(portFieldSumNode);
    BOOST_CHECK_EQUAL(portFieldSumNode->getPortName(), "port1");
    BOOST_CHECK_EQUAL(portFieldSumNode->getFieldName(), "field1");

    expression = "port2.field1";
    BOOST_CHECK_THROW(convertExpressionToNode(expression, model), InputError);
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
    const auto expressionWithTimeIndex = "(" + expression + ")" + "[param2]";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* time_index = registry.create<Nodes::ParameterNode>("param2");
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
    const auto expressionWithTimeIndex = "(" + expression + ")" + "[t-89*param2]";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* lit = registry.create<Nodes::LiteralNode>(89);
    auto* neg = registry.create<Nodes::NegationNode>(lit);

    auto* param = registry.create<Nodes::ParameterNode>("param2");
    auto* mult = registry.create<Nodes::MultiplicationNode>(neg, param);
    auto* timeShiftNode = registry.create<Nodes::TimeShiftNode>(div, mult);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeShiftNode));
}

BOOST_FIXTURE_TEST_CASE(TimeSumExpression, RegistryHolder)
{
    const auto [e, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "sum(t-89*param2 .. t+(param2/89)," + e + ")";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* lit = registry.create<Nodes::LiteralNode>(89);
    auto* neg = registry.create<Nodes::NegationNode>(lit);

    auto* param = registry.create<Nodes::ParameterNode>("param2");
    auto* from = registry.create<Nodes::TPlusNode>(
      registry.create<Nodes::MultiplicationNode>(neg, param));
    auto* to = registry.create<Nodes::TPlusNode>(registry.create<Nodes::DivisionNode>(param, lit));
    const auto* timeSumNode = registry.create<Nodes::TimeSumNode>(from, to, div);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeSumNode));
}

BOOST_FIXTURE_TEST_CASE(TimeSumExpressionWithFixedBounds, RegistryHolder)
{
    const auto [e, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "sum(1 .. 2," + e + ")";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* from = registry.create<Nodes::LiteralNode>(1);
    auto* to = registry.create<Nodes::LiteralNode>(2);
    const auto* timeSumNode = registry.create<Nodes::TimeSumNode>(from, to, div);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeSumNode));
}

BOOST_FIXTURE_TEST_CASE(TimeSumExpressionWithMixedBounds, RegistryHolder)
{
    const auto [e, div] = expected_expression(registry);
    const auto expressionWithTimeIndex = "sum(1 .. t+2," + e + ")";
    auto expr = convertExpressionToNode(expressionWithTimeIndex, createYmlModel());

    auto* from = registry.create<Nodes::LiteralNode>(1);
    auto* to = registry.create<Nodes::TPlusNode>(registry.create<Nodes::LiteralNode>(2));
    const auto* timeSumNode = registry.create<Nodes::TimeSumNode>(from, to, div);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, timeSumNode));
}

BOOST_AUTO_TEST_CASE(time_sum_and_time_shift_accept_right_side_multiplication)
{
    const auto model = createYmlModel();
    BOOST_CHECK_NO_THROW(convertExpressionToNode("sum(t + 1 - param2 / 2 .. t, varP)", model));
    BOOST_CHECK_NO_THROW(convertExpressionToNode("varP[t + 1 - param2 / 2]", model));
}

BOOST_AUTO_TEST_CASE(time_bounds_reject_solution_operators_and_nested_variables)
{
    auto model = createYmlModel();
    model.constraints = {{"constraintA", YmlModel::ExpressionLineNumber{"", 0}, "test.yaml", ""}};
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("sum(dual(constraintA) .. 2, varP)", model),
                          InputError,
                          containsMessage("Time-sum `from` bound cannot contain 'dual' operators"));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[reduced_cost(varP)]", model),
                          InputError,
                          containsMessage(
                            "Time index arguments cannot contain 'reduced_cost' operators"));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[t + (1 + dual(constraintA))]", model),
                          InputError,
                          containsMessage("Time shift arguments cannot contain 'dual' operators"));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[t + (1 + varP)]", model),
                          InputError,
                          containsMessage("Time shift arguments cannot contain variables"));
}

BOOST_AUTO_TEST_CASE(time_sum_bounds_reject_variables)
{
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("sum(varP .. 2, varP)", createYmlModel()),
                          InputError,
                          containsMessage(
                            "Time-sum `from` bound cannot contain variables: 'varP'."));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("sum(1 .. varP, varP)", createYmlModel()),
                          InputError,
                          containsMessage("Time-sum `to` bound cannot contain variables: 'varP'."));
}

BOOST_FIXTURE_TEST_CASE(time_index_accepts_parameter_expression, RegistryHolder)
{
    const auto expr = convertExpressionToNode("varP[param2 + 1]", createYmlModel());

    auto* variable = registry.create<Nodes::VariableNode>("varP", 0);
    auto* parameter = registry.create<Nodes::ParameterNode>("param2");
    auto* one = registry.create<Nodes::LiteralNode>(1);
    auto* index = registry.create<Nodes::SumNode>(parameter, one);
    auto* expected = registry.create<Nodes::TimeIndexNode>(variable, index);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, expected));
}

BOOST_FIXTURE_TEST_CASE(time_index_accepts_literal_expression, RegistryHolder)
{
    const auto expr = convertExpressionToNode("varP[1 + 2]", createYmlModel());

    auto* variable = registry.create<Nodes::VariableNode>("varP", 0);
    auto* one = registry.create<Nodes::LiteralNode>(1);
    auto* two = registry.create<Nodes::LiteralNode>(2);
    auto* index = registry.create<Nodes::SumNode>(one, two);
    auto* expected = registry.create<Nodes::TimeIndexNode>(variable, index);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, expected));
}

BOOST_AUTO_TEST_CASE(dual_and_reduced_cost_are_rejected_recursively_in_time_sum_bounds)
{
    auto model = createYmlModel();
    model.constraints.push_back(
      {"constraintA", YmlModel::ExpressionLineNumber{"", 0}, "test.yaml", ""});

    BOOST_CHECK_EXCEPTION(convertExpressionToNode("sum(1 + dual(constraintA) .. 2, varP)", model),
                          InputError,
                          containsMessage("Time-sum `from` bound cannot contain 'dual' operators"));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("sum(1 + reduced_cost(varP) .. 2, varP)", model),
                          InputError,
                          containsMessage(
                            "Time-sum `from` bound cannot contain 'reduced_cost' operators"));
}

BOOST_AUTO_TEST_CASE(time_shift_and_index_arguments_reject_variables)
{
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[t + varP]", createYmlModel()),
                          InputError,
                          containsMessage("Time shift arguments cannot contain variables"));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[varP]", createYmlModel()),
                          InputError,
                          containsMessage("Time index arguments cannot contain variables"));
}

BOOST_AUTO_TEST_CASE(time_arguments_reject_time_dependent_parameters_at_load_time)
{
    auto model = createYmlModel();
    const std::pair<std::string, std::string> cases[] = {
      {"varP[param1]", "Time index arguments"},
      {"(varP + 1)[1 + param1 / 2]", "Time index arguments"},
      {"varP[t + param1]", "Time shift arguments"},
      {"(varP + 1)[t - (1 + param1 / 2)]", "Time shift arguments"},
      {"sum(param1 .. 2, varP)", "Time-sum `from` bound"},
      {"sum(1 .. t + (param1 / 2), varP)", "Time-sum `to` bound"}};

    for (const bool scenarioDependent: {false, true})
    {
        model.parameters[0].scenario_dependent = scenarioDependent;
        for (const auto& [expression, context]: cases)
        {
            BOOST_TEST_CONTEXT(expression << ", scenario-dependent: " << scenarioDependent)
            {
                BOOST_CHECK_EXCEPTION(convertExpressionToNode(expression, model, "test.yaml:42"),
                                      InputError,
                                      containsMessage(
                                        "in test.yaml:42 : Error while converting expression '"
                                        + expression + "': " + context
                                        + " cannot contain time-dependent parameters: 'param1'."));
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(time_arguments_accept_scenario_dependent_time_constant_parameters)
{
    auto model = createYmlModel();
    model.parameters[1].scenario_dependent = true;
    for (const std::string expression: {"varP[param2 + 1]",
                                        "(varP + 1)[param2]",
                                        "varP[t - param2]",
                                        "(varP + 1)[t + 1 - param2 / 2]",
                                        "sum(param2 .. t + param2, varP)"})
    {
        BOOST_TEST_CONTEXT(expression)
        {
            BOOST_CHECK_NO_THROW(convertExpressionToNode(expression, model));
        }
    }
}

BOOST_AUTO_TEST_CASE(time_arguments_reject_port_fields)
{
    auto model = createYmlModel();
    model.ports.push_back({.id = "port", .type = "blue"});

    for (const std::string portExpression: {"port.flow", "sum_connections(port.flow)"})
    {
        BOOST_TEST_CONTEXT(portExpression)
        {
            BOOST_CHECK_EXCEPTION(
              convertExpressionToNode("varP[" + portExpression + "]", model),
              InputError,
              containsMessage("Time index arguments can only contain literals and parameters."));
            BOOST_CHECK_EXCEPTION(
              convertExpressionToNode("(varP + 1)[1 + " + portExpression + "]", model),
              InputError,
              containsMessage("Time index arguments can only contain literals and parameters."));
            BOOST_CHECK_EXCEPTION(
              convertExpressionToNode("varP[t + (" + portExpression + ")]", model),
              InputError,
              containsMessage("Time shift arguments can only contain literals and parameters."));
            BOOST_CHECK_EXCEPTION(
              convertExpressionToNode("(varP + 1)[t - (1 + " + portExpression + ")]", model),
              InputError,
              containsMessage("Time shift arguments can only contain literals and parameters."));
            BOOST_CHECK_EXCEPTION(
              convertExpressionToNode("sum(" + portExpression + " .. 2, varP)", model),
              InputError,
              containsMessage("Time-sum `from` bound can only contain literals and parameters."));
            BOOST_CHECK_EXCEPTION(
              convertExpressionToNode("sum(1 .. t + (" + portExpression + "), varP)", model),
              InputError,
              containsMessage("Time-sum `to` bound can only contain literals and parameters."));
        }
    }
}

BOOST_AUTO_TEST_CASE(dual_and_reduced_cost_are_rejected_recursively_in_shifts_and_indices)
{
    auto model = createYmlModel();
    model.constraints.push_back(
      {"constraintA", YmlModel::ExpressionLineNumber{"", 0}, "test.yaml", ""});

    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[t + (1 * dual(constraintA))]", model),
                          InputError,
                          containsMessage("Time shift arguments cannot contain 'dual' operators"));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[t + (1 * reduced_cost(varP))]", model),
                          InputError,
                          containsMessage(
                            "Time shift arguments cannot contain 'reduced_cost' operators"));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[1 + dual(constraintA)]", model),
                          InputError,
                          containsMessage("Time index arguments cannot contain 'dual' operators"));
    BOOST_CHECK_EXCEPTION(convertExpressionToNode("varP[1 + reduced_cost(varP)]", model),
                          InputError,
                          containsMessage(
                            "Time index arguments cannot contain 'reduced_cost' operators"));
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

struct SupplyModelForFunctionalOperator
{
    YmlModel::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"pmin", true, false}},
      .variables = {{"varA",
                     YmlModel::ExpressionLineNumber{"7", 0},
                     YmlModel::ExpressionLineNumber{"pmin", 0},
                     YmlModel::ValueType::CONTINUOUS,
                     false,
                     false,
                     "test.yaml"},
                    {"varB",
                     YmlModel::ExpressionLineNumber{"7", 0},
                     YmlModel::ExpressionLineNumber{"pmin", 0},
                     YmlModel::ValueType::CONTINUOUS,
                     false,
                     false,
                     "test.yaml"}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {{"constraintA", YmlModel::ExpressionLineNumber{"", 0}, "test.yaml", ""}},
      .binding_constraints = {{"constraintB",
                               YmlModel::ExpressionLineNumber{"", 0},
                               "test.yaml",
                               ""}},
      .objectives = {{"objective-id", YmlModel::ExpressionLineNumber{"", 0}, "test.yaml"}},
      .extra_outputs = {},
      .filename = ""};

    ForbiddenNodes forbiddenNodes;
};

BOOST_FIXTURE_TEST_CASE(dualExpression, SupplyModelForFunctionalOperator)
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
                          InputError,
                          containsMessage(expected_msg));
}

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
                          InputError,
                          containsMessage(err_msg));
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
    std::string expression = "max(varB, 2, pmin, varA, varB^pmin)";

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

// ─────────────────────────────────────────────
// Operators with an invalid argument list
// ─────────────────────────────────────────────

namespace
{
struct InvalidExpressionCase
{
    std::string expression;
    std::string expectedMessage;
};

std::ostream& operator<<(std::ostream& os, const InvalidExpressionCase& c)
{
    return os << c.expression;
}
} // namespace

BOOST_DATA_TEST_CASE_F(
  SupplyModelForFunctionalOperator,
  invalid_operator_arguments_are_rejected,
  boost::unit_test::data::make<InvalidExpressionCase>({
    {"dual()", "dual operator expects an argument, got nothing"},
    {"dual(constraintA, e^(iPi) + 1 = 0)",
     "dual operator expects exactly one constraint id got: constraintA, e^(iPi)+1=0"},
    {"reduced_cost()", "reduced_cost operator expects an argument, got nothing"},
    {"reduced_cost(varB, 2)", "reduced_cost operator expects exactly one variable id got: varB, 2"},
    {"varA^_", "No parameter or variable found for this identifier: _"},
    {"max(varB)", "max operator expects at least 2 operands got 1"},
    {"min(varB)", "min operator expects at least 2 operands got 1"},
    {"floor()", "floor operator expects an argument, got nothing"},
    {"floor(pmin, 5)", "floor() expects 1 argument, but has 2"},
    {"ceil()", "ceil operator expects an argument, got nothing"},
    {"ceil(pmin, 5)", "ceil() expects 1 argument, but has 2"},
    {"round()", "round operator expects an argument, got nothing"},
    {"round(pmin, 5)", "round() expects 1 argument, but has 2"},
    {"abs()", "abs operator expects an argument, got nothing"},
    {"abs(pmin, 5)", "abs() expects 1 argument, but has 2"},
  }),
  c)
{
    BOOST_CHECK_EXCEPTION(convertExpressionToNode(c.expression, model),
                          InputError,
                          containsMessage(c.expectedMessage));
}

// ─────────────────────────────────────────────
// Forbidden nodes
// ─────────────────────────────────────────────

namespace
{
struct ForbiddenNodeCase
{
    std::string expression;
    std::function<void(ForbiddenNodes&)> forbid;
    std::string expectedMessage;
};

std::ostream& operator<<(std::ostream& os, const ForbiddenNodeCase& c)
{
    return os << c.expression;
}

template<Nodes::FunctionNodeType type>
ForbiddenNodeCase variableForbiddenInFunction(const std::string& expression, const std::string& fn)
{
    return {expression,
            [](ForbiddenNodes& f) { f.parentForbidsChild<type, Nodes::VariableNode>(); },
            "'FunctionNode::" + fn + "' is not allowed to contain 'VariableNode' in expression '"
              + expression + "'"};
}

template<class NodeType>
ForbiddenNodeCase forbiddenGlobally(const std::string& expression, const std::string& nodeName)
{
    return {expression,
            [](ForbiddenNodes& f) { f.forbidGlobally<NodeType>(); },
            "'" + nodeName + "' is not allowed in expression '" + expression + "'"};
}
} // namespace

BOOST_DATA_TEST_CASE_F(
  SupplyModelForFunctionalOperator,
  forbidden_nodes_are_rejected,
  boost::unit_test::data::make<ForbiddenNodeCase>({
    variableForbiddenInFunction<Nodes::FunctionNodeType::min>("min(varB, reduced_cost(varB))",
                                                              "min"),
    variableForbiddenInFunction<Nodes::FunctionNodeType::max>("max(reduced_cost(varB), pmin, varA)",
                                                              "max"),
    variableForbiddenInFunction<Nodes::FunctionNodeType::floor>("floor(varA)", "floor"),
    variableForbiddenInFunction<Nodes::FunctionNodeType::ceil>("ceil(varA)", "ceil"),
    variableForbiddenInFunction<Nodes::FunctionNodeType::round>("round(varA)", "round"),
    variableForbiddenInFunction<Nodes::FunctionNodeType::abs>("abs(varA)", "abs"),
    forbiddenGlobally<Nodes::LessThanOrEqualNode>("varA <= 38", "LessThanOrEqualNode"),
    forbiddenGlobally<Nodes::GreaterThanOrEqualNode>("varA >= 38", "GreaterThanOrEqualNode"),
    forbiddenGlobally<Nodes::EqualNode>("varA = 38", "EqualNode"),
  }),
  c)
{
    auto node = convertExpressionToNode(c.expression, model);
    c.forbid(forbiddenNodes);
    BOOST_CHECK_EXCEPTION(ForbiddenNodesVisitor(forbiddenNodes, c.expression).dispatch(node.node),
                          InputError,
                          containsMessage(c.expectedMessage));
}

// ─────────────────────────────────────────────
// Unary operators: floor, ceil, round, abs
// ─────────────────────────────────────────────

BOOST_DATA_TEST_CASE_F(SupplyModelForFunctionalOperator,
                       unary_operator_on_a_parameter,
                       boost::unit_test::data::make<std::string>({"floor", "ceil", "round", "abs"}),
                       op)
{
    auto expr = convertExpressionToNode(op + "(pmin)", model);

    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::" + op);
    const auto* function = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_REQUIRE(function);
    BOOST_CHECK_EQUAL(function->typeToString(), op);
    BOOST_REQUIRE_EQUAL(function->getOperands().size(), 1);
    const auto* param = dynamic_cast<Nodes::ParameterNode*>(function->getOperands()[0]);
    BOOST_REQUIRE(param);
    BOOST_CHECK_EQUAL(param->value(), "pmin");
}

BOOST_DATA_TEST_CASE_F(SupplyModelForFunctionalOperator,
                       unary_operator_on_a_literal,
                       boost::unit_test::data::make<std::string>({"floor", "ceil", "round", "abs"}),
                       op)
{
    auto expr = convertExpressionToNode(op + "(3.7)", model);

    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::" + op);
    const auto* function = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_REQUIRE(function);
    BOOST_REQUIRE_EQUAL(function->getOperands().size(), 1);
    const auto* literal = dynamic_cast<Nodes::LiteralNode*>(function->getOperands()[0]);
    BOOST_REQUIRE(literal);
    BOOST_CHECK_EQUAL(literal->value(), 3.7);
}

BOOST_FIXTURE_TEST_CASE(abs_operator_applied_to_negative_literal, SupplyModelForFunctionalOperator)
{
    auto expr = convertExpressionToNode("abs(-3.7)", model);

    BOOST_CHECK_EQUAL(expr.node->name(), "FunctionNode::abs");
    const auto* absNode = dynamic_cast<Nodes::FunctionNode*>(expr.node);
    BOOST_REQUIRE(absNode);
    BOOST_REQUIRE_EQUAL(absNode->getOperands().size(), 1);
    // Negative literals are represented as NegationNode(LiteralNode(3.7))
    const auto* negNode = dynamic_cast<Nodes::NegationNode*>(absNode->getOperands()[0]);
    BOOST_REQUIRE(negNode);
    const auto* literalNode = dynamic_cast<Nodes::LiteralNode*>(negNode->child());
    BOOST_REQUIRE(literalNode);
    BOOST_CHECK_EQUAL(literalNode->value(), 3.7);
}

BOOST_AUTO_TEST_SUITE_END()
