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
#define BOOST_TEST_MODULE toybox

#define WIN32_LEAN_AND_MEAN

#include <ANTLRInputStream.h>
#include <TokenStream.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/expressions/Registry.hxx>
#include "antares/antlr-interface/ExprLexer.h"
#include "antares/antlr-interface/ExprParser.h"
#include "antares/antlr-interface/ExprVisitor.h"
#include "antares/solver/expressions/nodes/VariableNode.h"
#include "antares/solver/expressions/visitors/PrintVisitor.h"

#include "yaml-cpp/yaml.h"

struct Model;
struct Parameter;

struct Library
{
    std::string id;
    std::string description;
    std::vector<Model> models;
};

struct Model
{
    std::string id;
    std::string description;
    std::vector<Parameter> parameters;
    std::string objective;
};

struct Parameter
{
    std::string name;
    bool time_dependent;
    bool scenario_dependent;
};

// Decode and encode structs
namespace YAML
{
template<>
struct convert<Library>
{
    static Node encode(const Library& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["description"] = rhs.description;
        node["models"] = rhs.models;
        return node;
    }

    static bool decode(const Node& node, Library& rhs)
    {
        if (!node.IsMap() || node.size() != 3)
        {
            return false;
        }

        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>();
        rhs.models = node["models"].as<std::vector<Model>>();
        return true;
    }
};

// Model
template<>
struct convert<Model>
{
    static Node encode(const Model& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["description"] = rhs.description;
        node["parameters"] = rhs.parameters;
        node["objective"] = rhs.objective;
        return node;
    }

    static bool decode(const Node& node, Model& rhs)
    {
        if (!node.IsMap() || node.size() != 4)
        {
            return false;
        }

        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>();
        rhs.parameters = node["parameters"].as<std::vector<Parameter>>();
        rhs.objective = node["objective"].as<std::string>();
        return true;
    }
};

// Parameter
template<>
struct convert<Parameter>
{
    static Node encode(const Parameter& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["time-dependent"] = rhs.time_dependent;
        node["scenario-dependent"] = rhs.scenario_dependent;
        return node;
    }

    static bool decode(const Node& node, Parameter& rhs)
    {
        if (!node.IsMap() || node.size() != 3)
        {
            return false;
        }

        rhs.name = node["name"].as<std::string>();
        rhs.time_dependent = node["time-dependent"].as<bool>();
        rhs.scenario_dependent = node["scenario-dependent"].as<bool>();
        return true;
    }
};

struct Component
{
    std::string id;
    std::string model;
    std::vector<Parameter> parameters;
};

// Component
template<>
struct convert<Component>
{
    static Node encode(const Component& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["model"] = rhs.model;
        node["parameters"] = rhs.parameters;
        return node;
    }

    static bool decode(const Node& node, Component& rhs)
    {
        if (!node.IsMap() || node.size() != 3)
        {
            return false;
        }

        rhs.id = node["id"].as<std::string>();
        rhs.model = node["model"].as<std::string>();
        rhs.parameters = node["parameters"].as<std::vector<Parameter>>();
        return true;
    }
};
} // namespace YAML

class PrintVisitor: public antlr4::tree::ParseTreeVisitor
{
public:
    virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree* node) override
    {
        for (auto child: node->children)
        {
            child->accept(this);
        }
        return antlrcpp::Any();
    }

    std::any visit(antlr4::tree::ParseTree* tree) override
    {
        std::cout << "visit " << tree->getText() << "\n";
        return std::any();
    }

    std::any visitTerminal(antlr4::tree::TerminalNode* node) override
    {
        std::cout << "visitTerminal " << node->getText() << "\n";
        return std::any();
    }

    std::any visitErrorNode(antlr4::tree::ErrorNode* node) override
    {
        return std::any();
    }
};

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

// Visitor to convert terminal nodes to Antares::Solver::Nodes
class ConvertorVisitor: public ExprVisitor
{
public:
    ConvertorVisitor(Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry):
        registry_(registry)
    {
    }

    virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree* node) override
    {
        for (auto child: node->children)
        {
            child->accept(this);
        }
        return antlrcpp::Any();
    }

    std::any visit(antlr4::tree::ParseTree* tree) override
    {
        return tree->accept(this);
    }

    std::any visitTerminal(antlr4::tree::TerminalNode* node) override
    {
        return std::any();
    }

    std::any visitErrorNode(antlr4::tree::ErrorNode* node) override
    {
        return std::any();
    }

    std::any visitIdentifier(ExprParser::IdentifierContext* context) override
    {
        auto variable_node = registry_.create<Antares::Solver::Nodes::VariableNode>(
          context->getText());
        return variable_node;
    }

    std::any visitMuldiv(ExprParser::MuldivContext* context) override
    {
        auto left = std::any_cast<Antares::Solver::Nodes::VariableNode*>(visit(context->expr(0)));
        auto right = std::any_cast<Antares::Solver::Nodes::VariableNode*>(visit(context->expr(1)));
        auto mult_node = registry_.create<Antares::Solver::Nodes::MultiplicationNode>(left, right);
        return mult_node;
    }

    std::any visitFullexpr(ExprParser::FullexprContext* context) override
    {
        return std::any();
    }

    std::any visitShift(ExprParser::ShiftContext* context) override
    {
        return std::any();
    }

    std::any visitNegation(ExprParser::NegationContext* context) override
    {
        return std::any();
    }

    std::any visitExpression(ExprParser::ExpressionContext* context) override
    {
        return std::any();
    }

    std::any visitComparison(ExprParser::ComparisonContext* context) override
    {
        return std::any();
    }

    std::any visitAddsub(ExprParser::AddsubContext* context) override
    {
        return std::any();
    }

    std::any visitPortField(ExprParser::PortFieldContext* context) override
    {
        return std::any();
    }

    std::any visitNumber(ExprParser::NumberContext* context) override
    {
        return std::any();
    }

    std::any visitTimeIndex(ExprParser::TimeIndexContext* context) override
    {
        return std::any();
    }

    std::any visitTimeShift(ExprParser::TimeShiftContext* context) override
    {
        return std::any();
    }

    std::any visitFunction(ExprParser::FunctionContext* context) override
    {
        return std::any();
    }

    std::any visitTimeShiftRange(ExprParser::TimeShiftRangeContext* context) override
    {
        return std::any();
    }

    std::any visitTimeRange(ExprParser::TimeRangeContext* context) override
    {
        return std::any();
    }

    Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry_;
};

// Simple test case
BOOST_AUTO_TEST_CASE(test_case1)
{
    std::string library_s = R"(
library:
format-version: "1.0"
id: basic
description: Basic library

port-types:
  - id: flow
    description: A port which transfers power flow
    fields:
      - name: flow

models:
  - id: generator
    description: A basic generator model
    parameters:
      - name: cost
        time-dependent: false
        scenario-dependent: false
      - name: value
        time-dependent: false
        scenario-dependent: false
    objective: "cost * value"
)";

    std::string component1_s = R"(
"study:
nodes:
- id: small_generator
  model: generator
  parameters:
    cost: 10
    value: 100
")";

    YAML::Node library_doc = YAML::Load(library_s);
    YAML::Node component_doc = YAML::Load(component1_s);
    auto models = library_doc["models"].as<std::vector<Model>>();
    BOOST_CHECK_EQUAL(models.size(), 1);
    auto generator = models[0];
    BOOST_CHECK_EQUAL(generator.id, "generator");

    auto objective_string = generator.objective;
    antlr4::ANTLRInputStream input(objective_string);
    ExprLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();
    for (auto t: tokens.getTokens())
    {
        std::cout << t->toString() << std::endl;
    }
    ExprParser parser(&tokens);
    auto expression_context = parser.expr();
    std::cout << "first child " << expression_context->children[0]->getText() << std::endl;
    Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry;
    ConvertorVisitor expr_visitor(registry);
    auto node = std::any_cast<Antares::Solver::Nodes::MultiplicationNode*>(
      expression_context->accept(&expr_visitor));

    Antares::Solver::Visitors::PrintVisitor print_visitor;
    std::cout << "node " << print_visitor.dispatch(node) << std::endl;
}
