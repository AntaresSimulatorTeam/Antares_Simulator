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

#include <stdexcept>

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/modeler/data.h>
#include <antares/solver/modeler/loadFiles/loadFiles.h>

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Modeler::Config;

namespace Antares::Solver::LoadFiles
{

class LocationError final: public std::invalid_argument
{
public:
    explicit LocationError(const Model& model, const std::string& message):
        std::invalid_argument("In model '" + model.Id() + "' checking for locations: " + message)
    {
    }
};

void checkFunctionNode(Node& node, Model& model)
{
    // dual and reduced_cost can only be used in subproblems
    if (auto* functionNode = dynamic_cast<FunctionNode*>(&node); functionNode)
    {
        if (functionNode->type() == FunctionNodeType::reduced_cost)
        {
            const auto* varNode = dynamic_cast<VariableNode*>(functionNode->getOperands().at(0));
            for (const auto& variable: model.Variables())
            {
                if (variable.Id() == varNode->value()
                    && variable.location() != Location::SUBPROBLEMS)
                {
                    throw LocationError(
                      model,
                      "Error for variable: '" + varNode->value()
                        + "' reduced_cost can only be used in variables located in subproblems");
                }
            }
        }

        if (functionNode->type() == FunctionNodeType::dual)
        {
            // This node contains the constraint name
            const auto* n = dynamic_cast<ParameterNode*>(functionNode->getOperands().at(0));
            for (const auto& constraint: model.Constraints())
            {
                if (constraint.Id() == n->value() && constraint.location() != Location::SUBPROBLEMS)
                {
                    throw LocationError(
                      model,
                      "Error for constraint: '" + n->name()
                        + "' dual can only be used in constraints located in subproblems");
                }
            }
        }
    }
}

void checkExpression(Node* expression, const Location& location, Model& model)
{
    for (auto& node: AST(expression))
    {
        // base variable
        if (const auto* varNode = dynamic_cast<VariableNode*>(&node); varNode)
        {
            for (const auto& variable: model.Variables())
            {
                if (variable.Id() == varNode->value()
                    && !AreLocationsCompatible(variable.location(), location))
                {
                    throw LocationError(
                      model,
                      "Error for variable: '" + varNode->value()
                        + "' location doesn't match the expression location (variable location: "
                        + LocationToStr(variable.location())
                        + ", expression location: " + LocationToStr(location) + ")");
                }
            }
            continue;
        }

        // Portfields can contains variables, we recursively check their expressions
        if (const auto* n = dynamic_cast<PortFieldNode*>(&node); n)
        {
            PortFieldKey key(n->getPortName(), n->getFieldName());
            checkExpression(model.PortFieldDefinitions().at(key).Definition().RootNode(),
                            location,
                            model);
            continue;
        }

        if (const auto* portFieldSumNode = dynamic_cast<PortFieldSumNode*>(&node); portFieldSumNode)
        {
            for (const auto& connection: model.ComponentConnections())
            {
                auto* n = connection.component()->nodeAtPortField(portFieldSumNode->getPortName(),
                                                                  portFieldSumNode->getFieldName());
                checkExpression(n, location, *connection.component()->getModel());
            }
            continue;
        }

        checkFunctionNode(node, model);
    }
}

void checkModel(Model& model)
{
    for (const auto& constraint: model.Constraints())
    {
        checkExpression(constraint.expression().RootNode(), constraint.location(), model);
    }

    for (const auto& objective: model.Objectives())
    {
        checkExpression(objective.expression().RootNode(), objective.location(), model);
    }

    // Extra outputs must be evaluated, they need to contain only subproblem objects
    for (const auto& [_, extraOutput]: model.ExtraOutputs())
    {
        checkExpression(extraOutput.expression().RootNode(), Location::SUBPROBLEMS, model);
    }
}

void checkLocations(Modeler::Data& data)
{
    for (auto& lib: data.libraries)
    {
        for (auto& [modelName, model]: lib.Models())
        {
            checkModel(model);
        }
    }
}

} // namespace Antares::Solver::LoadFiles
