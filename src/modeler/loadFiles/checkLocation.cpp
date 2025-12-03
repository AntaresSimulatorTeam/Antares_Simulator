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

#include <fmt/format.h>

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/PrintVisitor.h>
#include <antares/solver/modeler/data.h>
#include <antares/solver/modeler/loadFiles/checkLocation.h>

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Modeler::Config;

namespace Antares::Modeler::Checks
{

void checkModel(Model& model) // TODO use const
{
    for (const auto& constraint: model.Constraints())
    {
        checkExpression(constraint.expression().RootNode(),
                        constraint.location(),
                        model,
                        constraint.expression().Value());
    }

    for (const auto& objective: model.Objectives())
    {
        checkExpression(objective.expression().RootNode(),
                        objective.location(),
                        model,
                        objective.expression().Value());
    }

    // Extra outputs must be evaluated, they need to contain only subproblem objects
    for (const auto& [_, extraOutput]: model.ExtraOutputs())
    {
        checkExpression(extraOutput.expression().RootNode(),
                        Location::SUBPROBLEMS,
                        model,
                        extraOutput.expression().Value());
    }
}

void checkLocations(Modeler::Data& data) // TODO use const
{
    for (auto& lib: data.libraries)
    {
        for (auto& [modelName, model]: lib.Models())
        {
            checkModel(model);
        }
    }
}

void checkFunctionNode(const Node& node, Model& model, const std::string& exprStr)
{
    // dual and reduced_cost can only be used in subproblems
    if (auto* functionNode = dynamic_cast<const FunctionNode*>(&node); functionNode)
    {
        if (functionNode->type() == FunctionNodeType::reduced_cost)
        {
            const auto* varNode = dynamic_cast<const VariableNode*>(
              functionNode->getOperands().at(0));
            for (const auto& variable: model.Variables())
            {
                if (variable.Id() == varNode->value()
                    && variable.location() != Location::SUBPROBLEMS)
                {
                    throw LocationError(fmt::format(
                      "Model '{}': In expression '{}': Error for variable '{}': reduced_cost can "
                      "only be used on variables located in subproblems",
                      model.Id(),
                      exprStr,
                      varNode->value()));
                }
            }
        }

        if (functionNode->type() == FunctionNodeType::dual)
        {
            // This node contains the constraint name
            const auto* n = dynamic_cast<const ParameterNode*>(functionNode->getOperands().at(0));
            for (const auto& constraint: model.Constraints())
            {
                if (constraint.Id() == n->value() && constraint.location() != Location::SUBPROBLEMS)
                {
                    throw LocationError(
                      fmt::format("Model '{}': In expression '{}': Error for constraint '{}': dual "
                                  "can only be used on constraints located in subproblems",
                                  model.Id(),
                                  exprStr,
                                  n->value()));
                }
            }
        }
    }
}

void checkExpression(const Node* expression,
                     const Location& location,
                     Model& model,
                     const std::string& exprStr,              // used for error msgs
                     const std::string& errorMsgForPortField) // used for error msgs
{
    for (const auto& node: ASTconst(expression))
    {
        // base variable
        if (const auto* varNode = dynamic_cast<const VariableNode*>(&node); varNode)
        {
            for (const auto& variable: model.Variables())
            {
                if (variable.Id() == varNode->value()
                    && !AreLocationsCompatibleForExpressions(variable.location(), location))
                {
                    throw LocationError(
                      fmt::format("{}Model '{}': In expression '{}': Error for variable '{}': "
                                  "Location doesn't match the expression location (variable "
                                  "location: {}, expression location: {})",
                                  errorMsgForPortField,
                                  model.Id(),
                                  exprStr,
                                  varNode->value(),
                                  LocationToStr(variable.location()),
                                  LocationToStr(location)));
                }
            }
            continue;
        }

        // Portfields can contains variables, we recursively check their expressions
        if (const auto* portFieldNode = dynamic_cast<const PortFieldNode*>(&node); portFieldNode)
        {
            // This code allows to have a clear message if one of the referenced expression contains
            // bad locations
            std::string msgInCaseOfError = fmt::format(
              "In model '{}': In expression '{}': this port field definition '{}.{}': is "
              "referencing an expression containing an incorrect location",
              model.Id(),
              exprStr,
              portFieldNode->getPortName(),
              portFieldNode->getFieldName());

            PortFieldKey key(portFieldNode->getPortName(), portFieldNode->getFieldName());
            auto& expr = model.PortFieldDefinitions().at(key).Definition();
            checkExpression(model.PortFieldDefinitions().at(key).Definition().RootNode(),
                            location,
                            model,
                            expr.Value(),
                            msgInCaseOfError);
            continue;
        }

        if (const auto* portFieldSumNode = dynamic_cast<const PortFieldSumNode*>(&node);
            portFieldSumNode)
        {
            // This code allows to have a clear message if one of the referenced expression contains
            // bad locations
            std::string msgInCaseOfError = fmt::format(
              "In model '{}': In expression '{}': this 'sum_connections({}.{})' is referencing an "
              "expression containing an incorrect location: ",
              model.Id(),
              exprStr,
              portFieldSumNode->getPortName(),
              portFieldSumNode->getFieldName());

            for (const auto& [portLocalId, connections]: model.ComponentsConnections())
            {
                if (portLocalId != portFieldSumNode->getPortName())
                {
                    continue;
                }
                for (const auto& connection: connections)
                {
                    auto* component = connection.component();
                    auto* port = connection.port();
                    const auto& expr = component->expressionAtPortField(
                      port->Id(),
                      portFieldSumNode->getFieldName());

                    checkExpression(expr.RootNode(),
                                    location,
                                    *connection.component()->getModel(),
                                    expr.Value(),
                                    msgInCaseOfError);
                }
            }
            continue;
        }

        // handle dual and reduced_cost
        checkFunctionNode(node, model, exprStr);
    }
}
} // namespace Antares::Modeler::Checks
