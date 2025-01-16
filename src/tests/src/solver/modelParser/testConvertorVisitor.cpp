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

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include "antares/solver/expressions/Registry.hxx"
#include "antares/solver/expressions/visitors/CompareVisitor.h"
#include "antares/solver/modelConverter/convertorVisitor.h"
#include "antares/solver/modelParser/Library.h"

using namespace Antares::Solver;

class ExpressionToNodeConvertorEmptyModel
{
public:
    ExpressionToNodeConvertorEmptyModel(ModelParser::Model&& model):
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
    const ModelParser::Model model_;
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
    ModelParser::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "pmin", ModelParser::ValueType::CONTINUOUS}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = "objectives"};
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
    ModelParser::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}},
      .variables = {{"varP", "7", "pmin", ModelParser::ValueType::CONTINUOUS}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = "objectives"};

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
    auto operands = nodeSum->getOperands();
    BOOST_CHECK_EQUAL(toLiteral(operands[0])->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(operands[1])->value(), 2);
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

BOOST_AUTO_TEST_CASE(medium_expression)
{
    ModelParser::Model model{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "param1", ModelParser::ValueType::CONTINUOUS}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = "objectives"};

    ExpressionToNodeConvertorEmptyModel converter(std::move(model));
    std::string expression = "(12 * (4 - 1) + param1) / -(42 + 3 + varP)";
    auto expr = converter.run(expression);

    Registry<Nodes::Node> registry;

    auto* param = registry.create<Nodes::ParameterNode>("param1");
    auto* var = registry.create<Nodes::VariableNode>("varP");
    auto* l3 = registry.create<Nodes::LiteralNode>(3);
    auto* l42 = registry.create<Nodes::LiteralNode>(42);
    auto* l1 = registry.create<Nodes::LiteralNode>(1);
    auto* l4 = registry.create<Nodes::LiteralNode>(4);
    auto* l12 = registry.create<Nodes::LiteralNode>(12);
    auto* sub = registry.create<Nodes::SubtractionNode>(l4, l1);
    auto* mult = registry.create<Nodes::MultiplicationNode>(l12, sub);
    auto* sum1 = registry.create<Nodes::SumNode>(mult, param);
    auto* sum2 = registry.create<Nodes::SumNode>(l42, l3);
    auto* sum3 = registry.create<Nodes::SumNode>(sum2, var);
    auto* neg = registry.create<Nodes::NegationNode>(sum3);
    auto* div = registry.create<Nodes::DivisionNode>(sum1, neg);

    Visitors::CompareVisitor cmp;
    BOOST_CHECK(cmp.dispatch(expr.node, div));
}
