// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/PrintVisitor.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/solver/modeler/checks/checkLocation.h>

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Solver::Config;

namespace Antares::Solver::Checks
{

void checkModel(const Model& model, const System& system)
{
    std::vector<std::string> errors;
    for (const auto& constraint: model.Constraints())
    {
        try
        {
            checkExpression(constraint.expression().RootNode(),
                            constraint.location(),
                            model,
                            system,
                            constraint.expression().Value());
        }

        catch (const LocationError& e)
        {
            errors.push_back(e.what());
        }
    }

    for (const auto& objective: model.Objectives())
    {
        try
        {
            checkExpression(objective.expression().RootNode(),
                            objective.location(),
                            model,
                            system,
                            objective.expression().Value());
        }
        catch (const LocationError& e)
        {
            errors.push_back(e.what());
        }
    }

    // Extra outputs must be evaluated, they need to contain only subproblem objects
    for (const auto& [_, extraOutput]: model.ExtraOutputs())
    {
        try
        {
            checkExpression(extraOutput.expression().RootNode(),
                            Location::SUBPROBLEMS,
                            model,
                            system,
                            extraOutput.expression().Value());
        }
        catch (const LocationError& e)
        {
            errors.push_back(e.what());
        }
    }

    if (!errors.empty())
    {
        std::string combined;
        for (const auto& err: errors)
        {
            combined += err + "\n";
        }
        throw LocationError(combined);
    }
}

void checkLocations(const Solver::ModelerData& data)
{
    for (const auto& lib: data.libraries)
    {
        for (const auto& [_, model]: lib.Models())
        {
            checkModel(model, *data.system);
        }
    }
}

void checkFunctionNode(const Node& node, const Model& model, const std::string& exprStr)
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
                     const Model& model,
                     const System& system,
                     const std::string& exprStr,              // used for error msgs
                     const std::string& errorMsgForPortField) // used for error msgs
{
    std::vector<std::string> errors;
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
                    errors.push_back(
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
            PortFieldKey key(portFieldNode->getPortName(), portFieldNode->getFieldName());
            auto it = model.PortFieldDefinitions().find(key);
            if (it == model.PortFieldDefinitions().end())
            {
                throw LocationError(fmt::format(
                  "Model '{}': In expression '{}': Port field '{}.{}' has no definition in this "
                  "model. If you want to reference a port field, it needs to be defined first.",
                  model.Id(),
                  exprStr,
                  portFieldNode->getPortName(),
                  portFieldNode->getFieldName()));
            }

            std::string msgInCaseOfError = fmt::format(
              "In model '{}': In expression '{}': this port field definition '{}.{}': is "
              "referencing an expression containing an incorrect location: ",
              model.Id(),
              exprStr,
              portFieldNode->getPortName(),
              portFieldNode->getFieldName());

            auto& expr = it->second.Definition();
            checkExpression(expr.RootNode(),
                            location,
                            model,
                            system,
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

            for (const auto& component: system.Components())
            {
                if (component.getModel()->Id() != model.Id())
                {
                    continue;
                }

                for (const auto& connection:
                     component.componentConnectionsViaPort(portFieldSumNode->getPortName()))
                {
                    auto* component = connection.component();
                    auto* port = connection.port();
                    const auto& expr = component->expressionAtPortField(
                      port->Id(),
                      portFieldSumNode->getFieldName());

                    checkExpression(expr.RootNode(),
                                    location,
                                    *connection.component()->getModel(),
                                    system,
                                    expr.Value(),
                                    msgInCaseOfError);
                }
            }
            continue;
        }

        // handle dual and reduced_cost
        checkFunctionNode(node, model, exprStr);
    }

    if (!errors.empty())
    {
        std::string combined;
        for (const auto& err: errors)
        {
            combined += err + "\n";
        }
        throw LocationError(combined);
    }
}
} // namespace Antares::Solver::Checks
