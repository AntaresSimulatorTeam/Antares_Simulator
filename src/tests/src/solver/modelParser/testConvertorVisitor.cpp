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

#include "antares/solver/expressions/Registry.hxx"
#include "antares/solver/expressions/visitors/AstDOTStyleVisitor.h"
#include "antares/solver/expressions/visitors/CompareVisitor.h"
#include "antares/solver/libObjectModel/library.h"
#include "antares/solver/modelConverter/convertorVisitor.h"
#include "antares/solver/modelConverter/modelConverter.h"
#include "antares/solver/modelParser/Library.h"

using namespace Antares::Solver;

struct Fixture
{
    ModelParser::Model model;
    Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry;
};

static Nodes::LiteralNode* toLiteral(Nodes::Node* n)
{
    return dynamic_cast<Nodes::LiteralNode*>(n);
}

BOOST_FIXTURE_TEST_CASE(empty_expression, Fixture)
{
    auto* node = ModelConverter::convertExpressionToNode("", registry, model);
    BOOST_CHECK_EQUAL(node, nullptr);
}

BOOST_FIXTURE_TEST_CASE(negation, Fixture)
{
    std::string expression = "-7";
    auto* n = ModelConverter::convertExpressionToNode(expression, registry, model);
    BOOST_CHECK_EQUAL(n->name(), "NegationNode");
    auto* nodeNeg = dynamic_cast<Nodes::NegationNode*>(n);
    BOOST_CHECK_EQUAL(toLiteral(nodeNeg->child())->value(), 7);
}

BOOST_FIXTURE_TEST_CASE(identifier, Fixture)
{
    ModelParser::Library library;
    ModelParser::Model model0{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "pmin", ModelParser::ValueType::CONTINUOUS}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = "objectives"};

    std::string expression = "param1";
    auto* n = ModelConverter::convertExpressionToNode(expression, registry, model0);
    BOOST_CHECK_EQUAL(n->name(), "ParameterNode");

    expression = "varP";
    n = ModelConverter::convertExpressionToNode(expression, registry, model0);
    BOOST_CHECK_EQUAL(n->name(), "VariableNode");
}

bool expectedMessage(const std::exception& ex)
{
    BOOST_CHECK_EQUAL(ex.what(),
                      std::string("No parameter or variable found for this identifier: abc"));
    return true;
}

BOOST_FIXTURE_TEST_CASE(identifierNotFound, Fixture)
{
    ModelParser::Library library;
    ModelParser::Model model0{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "pmin", ModelParser::ValueType::CONTINUOUS},
                    {"var2", "0", "param2", ModelParser::ValueType::CONTINUOUS}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = "objectives"};

    std::string expression = "abc"; // not a param or var
    BOOST_CHECK_EXCEPTION(ModelConverter::convertExpressionToNode(expression, registry, model0),
                          std::exception,
                          expectedMessage);
}

BOOST_FIXTURE_TEST_CASE(AddSub, Fixture)
{
    std::string expression = "1 + 2";
    auto* n = ModelConverter::convertExpressionToNode(expression, registry, model);
    BOOST_CHECK_EQUAL(n->name(), "SumNode");

    auto* nodeSum = dynamic_cast<Nodes::SumNode*>(n);
    auto operands = nodeSum->getOperands();
    BOOST_CHECK_EQUAL(toLiteral(operands[0])->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(operands[1])->value(), 2);

    expression = "6 - 3";
    n = ModelConverter::convertExpressionToNode(expression, registry, model);
    BOOST_CHECK_EQUAL(n->name(), "SubtractionNode");

    auto* nodeSub = dynamic_cast<Nodes::SubtractionNode*>(n);
    BOOST_CHECK_EQUAL(toLiteral(nodeSub->left())->value(), 6);
    BOOST_CHECK_EQUAL(toLiteral(nodeSub->right())->value(), 3);
}

BOOST_FIXTURE_TEST_CASE(mulDiv, Fixture)
{
    std::string expression = "1 * 2";
    auto* n = ModelConverter::convertExpressionToNode(expression, registry, model);
    BOOST_CHECK_EQUAL(n->name(), "MultiplicationNode");

    auto* nodeMult = dynamic_cast<Nodes::MultiplicationNode*>(n);
    BOOST_CHECK_EQUAL(toLiteral(nodeMult->left())->value(), 1);
    BOOST_CHECK_EQUAL(toLiteral(nodeMult->right())->value(), 2);

    expression = "6 / 3";
    n = ModelConverter::convertExpressionToNode(expression, registry, model);
    BOOST_CHECK_EQUAL(n->name(), "DivisionNode");

    auto* nodeDiv = dynamic_cast<Nodes::DivisionNode*>(n);
    BOOST_CHECK_EQUAL(toLiteral(nodeDiv->left())->value(), 6);
    BOOST_CHECK_EQUAL(toLiteral(nodeDiv->right())->value(), 3);
}

BOOST_FIXTURE_TEST_CASE(comparison, Fixture)
{
    std::string expression = "1 = 2";
    auto* n = ModelConverter::convertExpressionToNode(expression, registry, model);
    BOOST_CHECK_EQUAL(n->name(), "EqualNode");

    expression = "1 <= 5";
    n = ModelConverter::convertExpressionToNode(expression, registry, model);
    BOOST_CHECK_EQUAL(n->name(), "LessThanOrEqualNode");

    expression = "8364 >= 27";
    n = ModelConverter::convertExpressionToNode(expression, registry, model);
    BOOST_CHECK_EQUAL(n->name(), "GreaterThanOrEqualNode");

    auto* nodeGreater = dynamic_cast<Nodes::GreaterThanOrEqualNode*>(n);
    BOOST_CHECK_EQUAL(toLiteral(nodeGreater->left())->value(), 8364);
    BOOST_CHECK_EQUAL(toLiteral(nodeGreater->right())->value(), 27);
}

BOOST_FIXTURE_TEST_CASE(medium_expression, Fixture)
{
    ModelParser::Model model0{
      .id = "model0",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "param1", ModelParser::ValueType::CONTINUOUS}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = "objectives"};

    std::string expression = "(12 * (4 - 1) + param1) / -(42 + 3 + varP)";
    auto* n = ModelConverter::convertExpressionToNode(expression, registry, model0);

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
    BOOST_CHECK(cmp.dispatch(n, div));
}
