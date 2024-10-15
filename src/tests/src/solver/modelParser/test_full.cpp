
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

#include "antares/solver/expressions/Registry.hxx"
#define WIN32_LEAN_AND_MEAN

#include <ANTLRInputStream.h>
#include <TokenStream.h>
#include <yaml-cpp/exceptions.h>

#include <boost/test/unit_test.hpp>

#include "antares/solver/expressions/visitors/AstDOTStyleVisitor.h"
#include "antares/solver/expressions/visitors/PrintVisitor.h"
#include "antares/solver/libObjectModel/library.h"
#include "antares/solver/modelConverter/modelConverter.h"
#include "antares/solver/modelParser/Library.h"
#include "antares/solver/modelParser/parser.h"

#include "ConvertorVisitor.h"
#include "ExprLexer.h"
#include "ExprParser.h"
#include "enum_operators.h"

using namespace std::string_literals;
using namespace Antares::Solver;

class ModelPrinter
{
public:
    ModelPrinter(std::filesystem::path outPath, Library& lib):
        outPath_(outPath),
        lib_(lib)
    {
    }

    void write()
    {
        auto file = std::ofstream(outPath_);
        file << "digraph " + lib_.Id() + " {\n";
        file << "compound =true;\n";
        file << "node[style = filled]\n";
        file << lib_.Id() + " [label=\"" + lib_.Description()
                  + "\", shape=box, style=filled, fillcolor=lightblue]\n";
        for (const auto& [model_id, model]: lib_.Models())
        {
            auto model_name = model_id;
            if (model_name == "node")
            {
                model_name = "_node";
            }
            std::replace(model_name.begin(), model_name.end(), '-', '_');
            file << lib_.Id() << " -> " << model_name << ";\n"; // Link
            file << model_name << " [label=\"" << model_name
                 << "\", shape=box, style=filled, fillcolor=lightgreen]\n"; // Model node with style

            //            file << "style = dashed;\n";
            //            file << "fontsize = 16;\n";
            //            file << "color = lightgrey;\n";
            //            file << "node [shape=plaintext];\n\n";

            //            file << "legend_" << graph_name << " [ label =\" " << "model" << ": " <<
            //            graph_name
            //                 << "\"]\n";

            for (const auto& [port_id, port]: model.Ports())
            {
                auto name = port_id;
                std::replace(name.begin(), name.end(), ' ', '_');
                file << model_name << " -> " << name << ";\n";
                file << name << " [label=\"" << name
                     << "\", shape=box, style=filled, fillcolor=lightcoral]\n";
            }

            if (model.Variables().size() > 0)
            {
                // Create a cluster for variables
                file << "subgraph cluster_" << model_name + "_V" << "{\n";
                file << "label = \"Variables\";\n";
                file << "style = dashed;\n";
                file << "fontsize = 16;\n";
                file << "color = lightgrey;\n";
                file << "node [shape=plaintext];\n\n";
                // Invisible node to link model to cluster
                file << model_name + "dummy_V" << " [label=\"\", shape=point, style=invis]\n";

                for (const auto& [variable_id, variable]: model.Variables())
                {
                    auto name = variable_id;
                    std::replace(name.begin(), name.end(), ' ', '_');
                    file << model_name + "_V_" + name << " [label=\"" << name
                         << "\", shape=polygon, fillcolor=lightcoral]\n";
                }
                file << "}\n";
                file << model_name << " -> " << model_name + "dummy_V" << " [lhead=" << "cluster_"
                     << model_name + "_V" << "] ;\n";
            }
            if (model.Parameters().size() > 0)
            {
                // PArameters
                // Create a cluster for parameters
                file << "subgraph cluster_" << model_name + "_P" << "{\n";
                file << "label = \"Parameters\";\n";
                file << "style = dashed;\n";
                file << "fontsize = 16;\n";
                file << "color = lightgrey;\n";
                file << "node [shape=plaintext];\n\n";
                // Invisible node to link model to cluster
                file << model_name + "dummy_P" << " [label=\"\", shape=point, style=invis]\n";

                for (const auto& [parameter_id, parameter]: model.Parameters())
                {
                    auto name = parameter_id;
                    std::replace(name.begin(), name.end(), ' ', '_');

                    // file << model_name << " -> " << model_name + "_P_" + name << ";\n";
                    file << model_name + "_P_" + name << " [label=\"" << name
                         << "\", shape=ellipse, fillcolor=lightpink]\n";
                }
                // Link model node to cluster

                file << "}\n";
                file << model_name << " -> " << model_name + "dummy_P" << " [lhead=" << "cluster_"
                     << model_name + "_P" << "] ;\n";
            }

            for (const auto& [constraint_name, constraint]: model.getConstraints())
            {
                auto name = constraint_name;
                std::replace(name.begin(), name.end(), ' ', '_');
                file << model_name << " -> " << name << ";\n";
                file << name << " [label=\"" << name
                     << "\", shape=box, style=filled, fillcolor=lightsalmon]\n";

                antlr4::ANTLRInputStream input(constraint.expression().Value());
                ExprLexer lexer(&input);
                antlr4::CommonTokenStream tokens(&lexer);
                tokens.fill();
                for (auto t: tokens.getTokens())
                {
                    // Log for debug
                    std::cout << t->toString() << std::endl;
                }
                ExprParser parser(&tokens);
                auto expression_context = parser.expr();

                // Convert expression to Antares::Solver::Nodes expressions
                Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry;
                ConvertorVisitor expr_visitor(registry, model);
                auto tmp = expression_context->accept(&expr_visitor);
                auto node = std::any_cast<Antares::Solver::Nodes::Node*>(tmp);

                std::ostringstream sub_graph;

                Antares::Solver::Visitors::AstDOTStyleVisitor visitor;
                visitor.setGraphName("Expression_"s + name);
                visitor.setExpression(constraint.expression().Value());
                visitor(sub_graph, node);
                file << sub_graph.str();
                file << name << " -> " << "Expression_" + name + "1"
                     << " [lhead=" << "cluster_Expression_"s + name << "];\n ";
            }

            if (!model.Objective().Value().empty())
            {
                auto name = "Fonction_objective_" + model_name;
                file << model_name << " -> " << name << ";\n";
                file << name << " [label=\"" << model.Objective().Value()
                     << "\", shape=box, style=filled, fillcolor=lime]\n";

                antlr4::ANTLRInputStream input(model.Objective().Value());
                ExprLexer lexer(&input);
                antlr4::CommonTokenStream tokens(&lexer);
                tokens.fill();
                for (auto t: tokens.getTokens())
                {
                    // Log for debug
                    std::cout << t->toString() << std::endl;
                }
                ExprParser parser(&tokens);
                auto expression_context = parser.expr();

                // Convert expression to Antares::Solver::Nodes expressions
                Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry;
                ConvertorVisitor expr_visitor(registry, model);
                auto tmp = expression_context->accept(&expr_visitor);
                auto node = std::any_cast<Antares::Solver::Nodes::Node*>(tmp);

                std::ostringstream sub_graph;

                Antares::Solver::Visitors::AstDOTStyleVisitor visitor;
                visitor.setGraphName("Objective_"s + model_name);
                visitor.setExpression(model.Objective().Value());
                visitor(sub_graph, node);
                file << sub_graph.str();
                file << "Fonction_objective_" + model_name << " -> "
                     << "Objective_" + model_name + "1"
                     << " [lhead=" << "cluster_Objective_"s + model_name << "];\n ";
            }

            //            antlr4::ANTLRInputStream input(model.Objective().Value());
            //            ExprLexer lexer(&input);
            //            antlr4::CommonTokenStream tokens(&lexer);
            //            tokens.fill();
            //            for (auto t: tokens.getTokens())
            //            {
            //                // Log for debug
            //                std::cout << t->toString() << std::endl;
            //            }
            //            ExprParser parser(&tokens);
            //            auto expression_context = parser.expr();

            // Convert expression to Antares::Solver::Nodes expressions
            //            Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry;
            //            ConvertorVisitor expr_visitor(registry, model);
            //            auto tmp = expression_context->accept(&expr_visitor);
            //            auto node = std::any_cast<Antares::Solver::Nodes::Node*>(tmp);
            //
            //            std::ostringstream sub_graph;
            //
            //            Antares::Solver::Visitors::AstDOTStyleVisitor visitor;
            //            visitor.setGraphName("Objective_"s + model_name);
            //            visitor.setExpression(model.Objective().Value());
            //            visitor(sub_graph, node);
            //            file << sub_graph.str();
            //            file << "Fonction_objective_" + model_name << " -> " << "Objective_" +
            //            model_name + "1"
            //                 << " [lhead=" << "cluster_Obejective_"s + model_name << "];\n ";
        }

        file << "}\n";
    }

    std::filesystem::path outPath_;
    Library& lib_;
};

BOOST_AUTO_TEST_CASE(test_full)
{
    auto library = R"(
# Copyright (c) 2024, RTE (https://www.rte-france.com)
#
# See AUTHORS.txt
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# SPDX-License-Identifier: MPL-2.0
#
# This file is part of the Antares project.
library:
  id: basic
  description: Basic library

  port-types:
    - id: flow
      description: A port which transfers power flow
      fields:
        - id: flow

  models:
    - id: generator
      description: A basic generator model
      parameters:
        - id: cost
          time-dependent: false
          scenario-dependent: false
        - id: p_max
          time-dependent: false
          scenario-dependent: false
      variables:
        - id: generation
          lower-bound: 0
          upper-bound: p_max
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: generation
      objective: sum(cost * generation)


    - id: node
      description: A basic balancing node model
      ports:
        - id: injection_port
          type: flow
      binding-constraints:
        - id: balance
          expression:  sum_connections(injection_port.flow) = 0

    - id: spillage
      description: A basic spillage model
      parameters:
        - id: cost
          time-dependent: false
          scenario-dependent: false
      variables:
        - id: spillage
          lower-bound: 0
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: -spillage

    - id: unsupplied
      description: A basic unsupplied model
      parameters:
        - id: cost
          time-dependent: false
          scenario-dependent: false
      variables:
        - id: unsupplied_energy
          lower-bound: 0
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: unsupplied_energy

    - id: demand
      description: A basic fixed demand model
      parameters:
        - id: demand
          time-dependent: true
          scenario-dependent: true
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: -demand

    - id: short-term-storage
      description: A short term storage
      parameters:
        - id: efficiency
        - id: level_min
        - id: level_max
        - id: p_max_withdrawal
        - id: p_max_injection
        - id: inflows
      variables:
        - id: injection
          lower-bound: 0
          upper-bound: p_max_injection
        - id: withdrawal
          lower-bound: 0
          upper-bound: p_max_withdrawal
        - id: level
          lower-bound: level_min
          upper-bound: level_max
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: injection - withdrawal
      constraints:
        - id: Level equation
          expression: level[t] - level[t-1] - efficiency * injection + withdrawal = inflows

    - id: thermal-cluster-dhd
      description: DHD model for thermal cluster
      parameters:
        - id: cost
        - id: p_min
        - id: p_max
        - id: d_min_up
        - id: d_min_down
        - id: nb_units_max
        - id: nb_failures
          time-dependent: true
          scenario-dependent: true
      variables:
        - id: generation
          lower-bound: 0
          upper-bound: nb_units_max * p_max
          time-dependent: true
          scenario-dependent: true
        - id: nb_on
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
        - id: nb_stop
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
        - id: nb_start
          lower-bound: 0
          upper-bound: nb_units_max
          time-dependent: true
          scenario-dependent: false
      ports:
        - id: injection_port
          type: flow
      port-field-definitions:
        - port: injection_port
          field: flow
          definition: generation
      constraints:
        - id: Max generation
          expression: generation <= nb_on * p_max
        - id: Min generation
          expression: generation >= nb_on * p_min
        - id: Number of units variation
          expression: nb_on = nb_on[t-1] + nb_start - nb_stop
        - id: Min up time
          expression: sum(t_elem-d_min_up + 1) <= nb_on
        - id: Min down time
          expression: sum(t_elem-d_min_down + 1) <= nb_units_max[t-d_min_down] - nb_on
      objective: sum(cost * generation)

    )"s;

    try
    {
        const auto tmpDir = std::filesystem::temp_directory_path();
        const auto dirname = "dotdir"s; // std::tmpnam(nullptr);
        std::filesystem::create_directory(tmpDir / dirname);
        ModelParser::Parser parser;
        ModelParser::Library libraryObj = parser.parse(library);
        ObjectModel::Library lib = ModelConverter::convert(libraryObj);
        ModelPrinter printer(tmpDir / dirname / "lib.dot", lib);
        printer.write();
        //        for (auto& [model_id, model]: lib.Models())
        //        {
        //            for (auto& [containt_name, constraint]: model.getConstraints())
        //            {
        //                antlr4::ANTLRInputStream input(constraint.expression().Value());
        //                ExprLexer lexer(&input);
        //                antlr4::CommonTokenStream tokens(&lexer);
        //                tokens.fill();
        //                for (auto t: tokens.getTokens())
        //                {
        //                    // Log for debug
        //                    std::cout << t->toString() << std::endl;
        //                }
        //                ExprParser parser(&tokens);
        //                auto expression_context = parser.expr();
        //
        //                // Convert expression to Antares::Solver::Nodes expressions
        //                Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry;
        //                ConvertorVisitor expr_visitor(registry, model);
        //                auto tmp = expression_context->accept(&expr_visitor);
        //                auto node = std::any_cast<Antares::Solver::Nodes::Node*>(tmp);
        //
        //                // Create temp directory to store all dot file
        //                //                auto file_path = tmpDir / dirname / (model_id + "_"s +
        //                //                containt_name + ".dot"s); std::cout << "Writing to " <<
        //                file_path
        //                //                << std::endl; auto file_stream =
        //                std::ofstream(file_path); if
        //                //                (!file_stream.is_open())
        //                //                {
        //                //                    BOOST_FAIL("Cannot open file " +
        //                file_path.string());
        //                //                }
        //                //                Antares::Solver::Visitors::AstDOTStyleVisitor visitor;
        //                // visitor(file_stream, node);
        //            }
    }

    catch (const YAML::Exception& e)
    {
        std::cout << e.what() << std::endl;
        BOOST_FAIL(e.what());
    }

    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        BOOST_FAIL(e.what());
    }
}
