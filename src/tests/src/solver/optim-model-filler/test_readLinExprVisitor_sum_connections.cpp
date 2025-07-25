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

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/connection.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimization;

struct container_of_helpful_data_for_unit_tests
{
    SystemModel::Expression toExpression(Node* node, const std::string& value)
    {
        NodeRegistry nodeRegistry(node, std::move(registry));
        return SystemModel::Expression(value, std::move(nodeRegistry));
    }

    Registry<Node> registry;
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData data;
    Antares::Optimisation::LinearProblemApi::EmptyScenario empty_scenario;
    EvaluationContext evaluationContext{{}, {}, data, empty_scenario};
    SystemModel::ModelBuilder modelBuilder;
    SystemModel::ComponentBuilder componentBuilder;
};

BOOST_AUTO_TEST_SUITE(_running_the_read_linear_expression_visitor_on_a_sum_connections_)

BOOST_FIXTURE_TEST_CASE(sum_conections_connects_2_components_with_a_port_field,
                        container_of_helpful_data_for_unit_tests)
{
    // ============================
    // Model "generator" creation
    // ============================
    // Section variables
    // -----------------
    // Variable : "generation"
    // ... Add variable "generation" to model's variables
    std::vector<SystemModel::Variable> variables;
    variables.push_back({"generation", {}, {}, SystemModel::ValueType::FLOAT, {}, {}});

    // Section ports
    // -----------------
    SystemModel::PortField flow("flow");
    std::vector<SystemModel::PortField> portFields = {flow};
    SystemModel::PortType portType("some-port-type", std::move(portFields));
    SystemModel::Port injection_port("injection_port", portType);

    // Section port-field-definitions
    // ------------------------------
    Node* generation_node = registry.create<VariableNode>("generation");
    std::vector<SystemModel::PortFieldDefinition> portFieldDefs;
    portFieldDefs.push_back({injection_port, flow, toExpression(generation_node, "generation")});

    auto generatorModel = modelBuilder.withId("generator")
                            .withVariables(std::move(variables))
                            .withPorts({injection_port})
                            .withPortFieldDefinitions(std::move(portFieldDefs))
                            .build();

    // ==========================
    // Model "node" creation
    // ==========================
    // Section binding-constraints
    // ... Building the AST associated to binding constraint
    Node* sum_connections_node = registry.create<PortFieldSumNode>("injection_port", "flow");
    Node* zero_node = registry.create<LiteralNode>(0.);
    Node* equal_node = registry.create<EqualNode>(sum_connections_node, zero_node);
    // ...  building a constraint from the previous AST
    SystemModel::Constraint balance_constraint("balance", toExpression(equal_node, "balance"));

    std::vector<SystemModel::Constraint> constraints;
    constraints.push_back(std::move(balance_constraint));

    auto nodeModel = modelBuilder.withId("node")
                       .withPorts({injection_port})
                       .withConstraints(std::move(constraints))
                       .build();

    // ======================
    // System creation
    // ======================
    // Section components
    // ------------------
    auto generatorComponent = componentBuilder.withId("G")
                                .withModel(&generatorModel)
                                .withScenarioGroupId("scenario_group")
                                .build();
    auto nodeComponent = componentBuilder.withId("N")
                           .withModel(&nodeModel)
                           .withScenarioGroupId("scenario_group")
                           .build();

    // Section connexions
    // ------------------
    const std::string portId = injection_port.Id();
    generatorComponent.addComponentConnection(portId,
                                              ConnectionEnd(&nodeComponent, &injection_port));
    nodeComponent.addComponentConnection(portId,
                                         ConnectionEnd(&generatorComponent, &injection_port));

    // Visitor associated to component named "N"
    ReadLinearExpressionVisitor visitor{evaluationContext, {0, 0, 0}, nodeComponent};

    auto timeDependentLinExpr = visitor.dispatch(sum_connections_node);

    auto linear_expression = timeDependentLinExpr.GetLinearExpressions().at(0);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 0.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);
}

BOOST_FIXTURE_TEST_CASE(sum_conections_connects_3_components_with_a_port_field,
                        container_of_helpful_data_for_unit_tests)
{
    // =======================
    // Model "generator"
    // =======================
    // Section variables
    // -----------------
    // Variable : "generation"
    // ... Add variable "generation" to model's variables
    std::vector<SystemModel::Variable> variables;
    variables.push_back({"generation", {}, {}, SystemModel::ValueType::FLOAT, {}, {}});

    // Section ports
    // -----------------
    SystemModel::PortField flow("flow");
    std::vector<SystemModel::PortField> portFields = {flow};
    SystemModel::PortType portType("some-port-type", std::move(portFields));
    SystemModel::Port injection_port("injection_port", portType);

    // Section port-field-definitions
    // ------------------------------
    Node* generation_node = registry.create<VariableNode>("generation");
    std::vector<SystemModel::PortFieldDefinition> generatorPortFieldDefs;
    generatorPortFieldDefs.push_back(
      {injection_port, flow, toExpression(generation_node, "generation")});

    auto generatorModel = modelBuilder.withId("generator")
                            .withVariables(std::move(variables))
                            .withPorts({injection_port})
                            .withPortFieldDefinitions(std::move(generatorPortFieldDefs))
                            .build();

    // ====================
    // Model "node"
    // ====================
    // Section binding-constraints
    // ... Building the AST associated to binding constraint
    Node* sum_connections_node = registry.create<PortFieldSumNode>("injection_port", "flow");
    Node* zero_node = registry.create<LiteralNode>(0.);
    Node* equal_node = registry.create<EqualNode>(sum_connections_node, zero_node);
    // ...  building a constraint from the previous AST
    SystemModel::Constraint balance_constraint("balance", toExpression(equal_node, "balance"));

    std::vector<SystemModel::Constraint> constraints;
    constraints.push_back(std::move(balance_constraint));

    auto nodeModel = modelBuilder.withId("node")
                       .withPorts({injection_port})
                       .withConstraints(std::move(constraints))
                       .build();

    // ====================
    // Model "demand"
    // ====================
    // Section parameters
    // -------------------
    SystemModel::Parameter parameter("demand", {}, {});

    // Section port-field-definitions
    // ------------------------------
    Node* demand_node = registry.create<ParameterNode>("demand");
    Node* root = registry.create<NegationNode>(demand_node);

    std::vector<SystemModel::PortFieldDefinition> demandPortFieldDefs;
    demandPortFieldDefs.push_back({injection_port, flow, toExpression(root, "-demand")});

    auto demandModel = modelBuilder.withId("demand")
                         .withParameters({parameter})
                         .withPorts({injection_port})
                         .withPortFieldDefinitions(std::move(demandPortFieldDefs))
                         .build();

    // ======================
    // System
    // ======================
    // Section components
    // ------------------
    auto generatorComponent = componentBuilder.withId("G")
                                .withModel(&generatorModel)
                                .withScenarioGroupId("scenario_group")
                                .build();
    auto nodeComponent = componentBuilder.withId("N")
                           .withModel(&nodeModel)
                           .withScenarioGroupId("scenario_group")
                           .build();

    auto demandComponent = componentBuilder.withId("D")
                             .withModel(&demandModel)
                             .withParameterValues(
                               {{"demand", {"demand", ParameterType::CONSTANT, "5"}}})
                             .withScenarioGroupId("scenario_group")
                             .build();
    // Section connexions
    // ------------------
    const std::string portId = injection_port.Id();
    generatorComponent.addComponentConnection(portId,
                                              ConnectionEnd(&nodeComponent, &injection_port));
    nodeComponent.addComponentConnection(portId,
                                         ConnectionEnd(&generatorComponent, &injection_port));

    demandComponent.addComponentConnection(portId, ConnectionEnd(&nodeComponent, &injection_port));
    nodeComponent.addComponentConnection(portId, ConnectionEnd(&demandComponent, &injection_port));

    // Visitor associated to component named "N"
    ReadLinearExpressionVisitor visitor{evaluationContext, {0, 0, 0}, nodeComponent};

    auto timeDependentLinExpr = visitor.dispatch(sum_connections_node);

    auto linear_expression = timeDependentLinExpr.GetLinearExpressions().at(0);

    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);
    FullKey generationKey(generatorComponent.Id(), "generation", MCYearAndTime::MCYear{0}, 0);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().at(generationKey), 1.);
    BOOST_CHECK_EQUAL(linear_expression.offset(), -5.);
}

BOOST_AUTO_TEST_SUITE_END()
