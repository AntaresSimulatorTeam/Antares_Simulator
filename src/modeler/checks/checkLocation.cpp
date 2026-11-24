// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>
#include <vector>

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/PrintVisitor.h>
#include <antares/logs/logs.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/solver/modeler/checks/checkLocation.h>

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Solver::Config;

namespace Antares::Solver::Checks
{
namespace
{
void collectExpressionErrors(const Node* expression,
                             const Location& location,
                             const Model& model,
                             const System& system,
                             const std::string& exprStr,
                             const std::string& context,
                             std::vector<std::string>& errors);

void collectModelErrors(const Model& model, const System& system, std::vector<std::string>& errors)
{
    for (const auto& constraint: model.Constraints())
    {
        collectExpressionErrors(constraint.expression().RootNode(),
                                constraint.location(),
                                model,
                                system,
                                constraint.expression().Value(),
                                "",
                                errors);
    }

    for (const auto& objective: model.Objectives())
    {
        collectExpressionErrors(objective.expression().RootNode(),
                                objective.location(),
                                model,
                                system,
                                objective.expression().Value(),
                                "",
                                errors);
    }

    // Extra outputs must be evaluated, they need to contain only subproblem objects
    for (const auto& [_, extraOutput]: model.ExtraOutputs())
    {
        collectExpressionErrors(extraOutput.expression().RootNode(),
                                Location::SUBPROBLEMS,
                                model,
                                system,
                                extraOutput.expression().Value(),
                                "",
                                errors);
    }
}

void reportErrors(const std::vector<std::string>& errors, bool logErrors)
{
    if (errors.empty())
    {
        return;
    }

    std::string combined;
    for (const auto& error: errors)
    {
        if (logErrors)
        {
            logs.error() << error;
        }
        if (!combined.empty())
        {
            combined += '\n';
        }
        combined += error;
    }
    throw LocationError(combined);
}
} // namespace

void checkModel(const Model& model, const System& system)
{
    std::vector<std::string> errors;
    collectModelErrors(model, system, errors);
    reportErrors(errors, true);
}

void checkLocations(const Solver::ModelerData& data)
{
    std::vector<std::string> errors;
    for (const auto& lib: data.libraries)
    {
        for (const auto& [_, model]: lib.Models())
        {
            collectModelErrors(model, *data.system, errors);
        }
    }
    reportErrors(errors, true);
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

namespace
{
void collectExpressionErrors(const Node* expression,
                             const Location& location,
                             const Model& model,
                             const System& system,
                             const std::string& exprStr,
                             const std::string& context,
                             std::vector<std::string>& errors)
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
                    errors.push_back(
                      fmt::format("{}Model '{}': In expression '{}': Error for variable '{}': "
                                  "Location doesn't match the expression location (variable "
                                  "location: {}, expression location: {})",
                                  context,
                                  model.Id(),
                                  exprStr,
                                  varNode->value(),
                                  LocationToStr(variable.location()),
                                  LocationToStr(location)));
                }
            }
            continue;
        }

        if (const auto* portFieldSumNode = dynamic_cast<const PortFieldSumNode*>(&node);
            portFieldSumNode)
        {
            // This code allows to have a clear message if one of the referenced expression contains
            // bad locations
            std::string msgInCaseOfError = fmt::format(
              "{}In model '{}': In expression '{}': this 'sum_connections({}.{})' is referencing "
              "an "
              "expression containing an incorrect location: ",
              context,
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
                    const auto& component = connection.component();
                    const auto& port = connection.port();
                    const auto& expr = component.expressionAtPortField(
                      port.Id(),
                      portFieldSumNode->getFieldName());

                    collectExpressionErrors(expr.RootNode(),
                                            location,
                                            *component.getModel(),
                                            system,
                                            expr.Value(),
                                            msgInCaseOfError,
                                            errors);
                }
            }
            continue;
        }

        // handle dual and reduced_cost
        try
        {
            checkFunctionNode(node, model, exprStr);
        }
        catch (const LocationError& e)
        {
            errors.push_back(context + e.what());
        }
    }
}
} // namespace

void checkExpression(const Node* expression,
                     const Location& location,
                     const Model& model,
                     const System& system,
                     const std::string& exprStr,
                     const std::string& errorMsgForPortField)
{
    std::vector<std::string> errors;
    collectExpressionErrors(expression,
                            location,
                            model,
                            system,
                            exprStr,
                            errorMsgForPortField,
                            errors);
    reportErrors(errors, false);
}
} // namespace Antares::Solver::Checks
