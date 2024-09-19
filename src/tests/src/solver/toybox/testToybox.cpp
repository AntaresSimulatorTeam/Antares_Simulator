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
#include "antares/solver/expressions/visitors/CloneVisitor.h"
#include "antares/solver/expressions/visitors/PrintVisitor.h"

#include "ConvertorVisitor.h"
#include "ModelParser.h"
#include "yaml-cpp/yaml.h"

struct Model;
struct Parameter;

// struct StructName => DTO
// struct StructNameInstance => Objet métier/technique/whatever

struct LibraryInstance
{
    std::string id;
    std::string description;
    std::vector<Model> models;
};

struct ComponentParameter
{
    std::string name;
    double value;
};

struct Component
{
    std::string id;
    std::string model;
    std::vector<ComponentParameter> parameters;
};

struct ComponentInstance
{
    ComponentInstance(const Component& c)
    {
        for (auto& p: c.parameters)
        {
            parameters[p.name] = p.value;
        }
    }

    std::map<std::string, double> parameters;
};

// Decode and encode structs
namespace YAML
{
// ComponentParameter
template<>
struct convert<ComponentParameter>
{
    static Node encode(const ComponentParameter& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["value"] = rhs.value;
        return node;
    }

    static bool decode(const Node& node, ComponentParameter& rhs)
    {
        if (!node.IsMap() || node.size() != 2)
        {
            return false;
        }

        rhs.name = node["name"].as<std::string>();
        rhs.value = node["value"].as<double>();
        return true;
    }
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
        rhs.parameters = node["parameters"].as<std::vector<ComponentParameter>>();
        return true;
    }
};
} // namespace YAML

// namespace YAML

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

class ParameterSubstitutionVisitor: public Antares::Solver::Visitors::CloneVisitor
{
public:
    ParameterSubstitutionVisitor(Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry,
                                 const ComponentInstance& component):
        Antares::Solver::Visitors::CloneVisitor(registry),
        component_(component),
        registry_(registry)
    {
    }

    Antares::Solver::Nodes::Node* visit(const Antares::Solver::Nodes::ParameterNode* node) override
    {
        auto it = component_.parameters.find(node->value());
        if (it != component_.parameters.end())
        {
            return registry_.create<Antares::Solver::Nodes::LiteralNode>(it->second);
        }
        else
        {
            throw std::runtime_error("Parameter not found: " + node->name() + "in component ");
        }
    }

    const ComponentInstance& component_;
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

    // Load model library and components
    YAML::Node library_doc = YAML::Load(library_s);
    auto models = library_doc["models"].as<std::vector<Model>>();
    LibraryInstance library;

    library.models = models;

    BOOST_CHECK_EQUAL(models.size(), 1);
    auto generator = models[0];
    BOOST_CHECK_EQUAL(generator.id, "generator");

    auto objective_string = generator.objective;

    // Parse fonction objective expression
    antlr4::ANTLRInputStream input(objective_string);
    ExprLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();
    for (auto t: tokens.getTokens())
    {
        // Log for debug
        std::cout << t->toString() << std::endl;
    }
    ExprParser parser(&tokens);
    auto expression_context = parser.expr(); // Now we have our expression parsed following the
                                             // grammar for expression
    
    // Convert expression to Antares::Solver::Nodes expressions
    Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry;
    ConvertorVisitor expr_visitor(registry);
    auto node = std::any_cast<Antares::Solver::Nodes::MultiplicationNode*>(
      expression_context->accept(&expr_visitor));

    // Use visitor on expression
    Antares::Solver::Visitors::PrintVisitor print_visitor;
    std::cout << "Converted tree " << print_visitor.dispatch(node) << std::endl;

    // Create component
    std::string component1_s = R"(
study:
nodes:
  - id: small_generator
    model: generator
    parameters:
        - name: cost
          value: 10
        - name: value
          value: 20

)";

    YAML::Node component_doc = YAML::Load(component1_s);

    auto components = component_doc["nodes"].as<std::vector<Component>>();
    auto component = components[0];
    ComponentInstance generator_instance(component);
    ParameterSubstitutionVisitor substitution_visitor(registry, generator_instance);
    auto substituedTree = substitution_visitor.dispatch(node);
    std::cout << "Parameter substitued: " << print_visitor.dispatch(substituedTree) << std::endl;
}
