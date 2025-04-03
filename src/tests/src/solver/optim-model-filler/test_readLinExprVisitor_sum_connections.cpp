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

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/connection.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::Study;
using namespace Antares::Optimization;

struct test_context_builder
{
    SystemModel::Expression createExpression(Node* node, const std::string& value)
    {
        NodeRegistry nodeRegistry(node, std::move(registry));
        return SystemModel::Expression(value, std::move(nodeRegistry));
    }

    Registry<Node> registry;
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData data;
    EvaluationContext evaluationContext{{}, {}, data};
    SystemModel::ModelBuilder modelBuilder;
    SystemModel::ComponentBuilder componentBuilder;
};

BOOST_AUTO_TEST_SUITE(_running_the_read_linear_expression_visitor_on_a_sum_connections_)

BOOST_FIXTURE_TEST_CASE(sum_conections_connects_2_components_with_a_port_field,
                        test_context_builder)
{
    // ============================
    // Model "generator" creation
    // ============================
    // Section variables
    // -----------------
    // ... Variable : "generation"
    Node* generation_node = registry.create<VariableNode>("generation");

    // ... Define bounds for variable "generation"
    Node* ub_node = registry.create<LiteralNode>(1.); // Arbitrary value
    Node* lb_node = registry.create<LiteralNode>(0.);

    // ... Add variable "generation" to model's variables
    std::vector<SystemModel::Variable> variables;
    variables.push_back({"generation",
                         createExpression(lb_node, "lb"),
                         createExpression(ub_node, "ub"),
                         SystemModel::ValueType::FLOAT,
                         {},
                         {}});

    // Section ports
    // -----------------
    SystemModel::PortField flow("flow");
    std::vector<SystemModel::PortField> portFields = {flow};
    SystemModel::PortType portType("some-port-type", std::move(portFields));
    SystemModel::Port injection_port("injection_port", portType);

    // Section port-field-definitions
    // ------------------------------
    SystemModel::PortFieldDefinition portFieldDefinition(injection_port,
                                                         flow,
                                                         createExpression(generation_node,
                                                                          "generation"));

    std::vector<SystemModel::PortFieldDefinition> portFieldDefinitions;
    portFieldDefinitions.push_back(std::move(portFieldDefinition));

    auto generatorModel = modelBuilder.withId("generator")
                            .withVariables(std::move(variables))
                            .withPorts({injection_port})
                            .withPortFieldDefinitions(std::move(portFieldDefinitions))
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
    SystemModel::Constraint balance_constraint("balance", createExpression(equal_node, "balance"));

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
    SystemModel::ConnectionEntry connectionEntry_1(&generatorComponent, &injection_port, {});
    SystemModel::ConnectionEntry connectionEntry_2(&nodeComponent, &injection_port, {});
    SystemModel::Connection connection(connectionEntry_1, connectionEntry_2);
    std::vector<SystemModel::Connection> connections;
    connections.push_back(connection);

    // Visitor associated to component named "N"
    ReadLinearExpressionVisitor visitor{evaluationContext, {0, 0}, nodeComponent.Id(), connections};

    auto timeDependentLinExpr = visitor.dispatch(sum_connections_node);

    auto linear_expression = timeDependentLinExpr.GetLinearExpressions().at(0);
    BOOST_CHECK_EQUAL(linear_expression.offset(), 0.);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().size(), 1);

    FullKey fullKey(generatorComponent.Id(), "generation", 0, 0);
    BOOST_CHECK_EQUAL(linear_expression.coefPerVar().at(fullKey), 1.);
}

BOOST_AUTO_TEST_SUITE_END()
