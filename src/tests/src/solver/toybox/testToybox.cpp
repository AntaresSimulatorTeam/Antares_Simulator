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
#include "antares/solver/expressions/nodes/VariableNode.h"
#include "antares/solver/expressions/visitors/PrintVisitor.h"

#include "ComponentParser.h"
#include "ConvertorVisitor.h"
#include "Model.h"
#include "ModelParser.h"
#include "ParameterSubstitution.h"
#include "PrintVisitor.h"

// struct StructName => DTO
// struct StructNameInstance => Objet métier/technique/whatever

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
    objective: "cost * pMax"
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
    ConvertorVisitor expr_visitor(registry, generator);
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
