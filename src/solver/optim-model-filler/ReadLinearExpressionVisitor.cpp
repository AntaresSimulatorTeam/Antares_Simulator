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

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include "antares/study/system-model/component.h"
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;

namespace Antares::Optimization
{
ReadLinearExpressionVisitor::ReadLinearExpressionVisitor(
  EvaluationContext evalContext,
  Optimisation::LinearProblemApi::FillContext fillContext,
  const SystemModel::Component& component,
  unsigned int nbModelVariables):
    fillContext_(std::move(fillContext)),
    evalContext_(std::move(evalContext)),
    component_(component),
    evalVisitor_(evalContext_, fillContext_),
    nbModelVariables_(nbModelVariables)
{
}

std::string ReadLinearExpressionVisitor::name() const
{
    return "ReadLinearExpressionVisitor";
}

LinearExpression ReadLinearExpressionVisitor::visit(const SumNode* node)
{
    const auto& operands = node->getOperands();
    LinearExpression ret(EvaluationResult(0),
                         std::move(std::vector(nbModelVariables_, EvaluationResult(0))));
    for (auto* operand: operands)
    {
        ret += dispatch(operand);
    }
    return ret;
}

LinearExpression ReadLinearExpressionVisitor::visit(const SubtractionNode* node)
{
    auto ret = dispatch(node->left());
    ret -= dispatch(node->right()); // using -= operator avoid expensive copy
    return ret;
}

LinearExpression ReadLinearExpressionVisitor::visit(const MultiplicationNode* node)
{
    auto ret = dispatch(node->left());
    ret *= dispatch(node->right());
    return ret;
}

LinearExpression ReadLinearExpressionVisitor::visit(const DivisionNode* node)
{
    auto ret = dispatch(node->left());
    ret /= dispatch(node->right());
    return ret;
}

LinearExpression ReadLinearExpressionVisitor::visit(const EqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpression ReadLinearExpressionVisitor::visit(const LessThanOrEqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpression ReadLinearExpressionVisitor::visit(const GreaterThanOrEqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpression ReadLinearExpressionVisitor::visit(const NegationNode* node)
{
    return -dispatch(node->child());
}

LinearExpression ReadLinearExpressionVisitor::visit(const VariableNode* node)
{
    const auto& variables = component_.getModel()->Variables();
    const auto& it = std::ranges::find_if(variables,
                                          [&node](const auto& variable)
                                          { return variable.Id() == node->value(); });
    if (it == variables.end())
    {
        throw std::invalid_argument("Variable (" + node->value() + ") not found.");
    }
    const auto& modelVariablesGlobalIndices = component_.ModelVariablesGlobalIndices();
    const auto globalIndex = modelVariablesGlobalIndices.at(std::distance(variables.begin(), it));
    std::vector coefPerVar(nbModelVariables_, EvaluationResult(0));
    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        coefPerVar[globalIndex] = EvaluationResult(1);
        return {EvaluationResult(0.), std::move(coefPerVar)};
    }
    // only dependent

    auto nbTimeStep = fillContext_.getLocalLastTimeStep() - fillContext_.getLocalFirstTimeStep()
                      + 1;
    coefPerVar[globalIndex] = EvaluationResult(std::vector<double>(nbTimeStep, 1));
    return {EvaluationResult(0), std::move(coefPerVar)};
}

LinearExpression ReadLinearExpressionVisitor::visit(const ParameterNode* node)
{
    const auto systemParameter = evalContext_.getParameter(node->value());
    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        && systemParameter.type != ParameterType::CONSTANT)
    {
        throw std::invalid_argument(
          "Parameter " + node->value()
          + " is declared constant in time and scenario in library but not in system");
    }
    std::vector emptyCoefPerVar(nbModelVariables_, EvaluationResult(0));
    if (systemParameter.type == ParameterType::CONSTANT)
    {
        return {EvaluationResult(evalContext_.getSystemParameterValueAsDouble(node->value())),
                std::move(emptyCoefPerVar)};
    }
    // only dependent
    auto nbTimeStep = fillContext_.getLocalLastTimeStep() - fillContext_.getLocalFirstTimeStep()
                      + 1;
    std::vector<double> parameters(nbTimeStep, 0.);
    int idx = 0;
    for (auto localTimeStep = fillContext_.getLocalFirstTimeStep();
         localTimeStep <= fillContext_.getLocalLastTimeStep();
         ++localTimeStep)
    {
        auto globalTimeStep = fillContext_.getGlobalFirstTimeStep() + idx;
        parameters[idx] = evalContext_.getParameterValue(node->value(),
                                                         fillContext_.getYear(),
                                                         globalTimeStep);
        idx++;
    }
    return {EvaluationResult(std::move(parameters)), std::move(emptyCoefPerVar)};
}

LinearExpression ReadLinearExpressionVisitor::visit(const LiteralNode* node)
{
    std::vector coefPerVar(nbModelVariables_, EvaluationResult(0));
    return {EvaluationResult(node->value()), std::move(coefPerVar)};
}

LinearExpression ReadLinearExpressionVisitor::visit(const PortFieldNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

LinearExpression ReadLinearExpressionVisitor::visit(const PortFieldSumNode* node)
{
    auto& portId = node->getPortName();
    auto& fieldId = node->getFieldName();

    LinearExpression to_return(EvaluationResult(0),
                               std::move(std::vector(nbModelVariables_, EvaluationResult(0))));
    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        const EvaluationContext connectedComponentEvalContext(component->getParameterValues(),
                                                              {},
                                                              evalContext_.data(),
                                                              evalContext_.scenario());
        ReadLinearExpressionVisitor visitor(connectedComponentEvalContext,
                                            fillContext_,
                                            *component,
                                            nbModelVariables_);

        const Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return += visitor.dispatch(node);
    }

    return to_return;
}

LinearExpression ReadLinearExpressionVisitor::visit(const ComponentVariableNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentVariableNodes");
}

LinearExpression ReadLinearExpressionVisitor::visit(const ComponentParameterNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentParameterNodes");
}

LinearExpression ReadLinearExpressionVisitor::visit(const TimeShiftNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeShift = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return ret.ShiftLinearExpressions(timeShift);
}

LinearExpression ReadLinearExpressionVisitor::visit(const TimeIndexNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeIndex = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return ret[timeIndex];
}

LinearExpression ReadLinearExpressionVisitor::visit(const TimeSumNode* node)
{
    const auto expression = dispatch(node->expression());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto from = static_cast<int>(evalVisitor_.dispatch(node->from()).valueAsDouble());

    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto to = static_cast<int>(evalVisitor_.dispatch(node->to()).valueAsDouble());

    return expression.TimeSumLinearExpressions(from, to);
}

LinearExpression ReadLinearExpressionVisitor::visit(const AllTimeSumNode* node)
{
    const auto expression = dispatch(node->child());
    return expression.AllTimeSumLinearExpressions(fillContext_.getLocalLastTimeStep()
                                                  - fillContext_.getLocalFirstTimeStep() + 1);
}
} // namespace Antares::Optimization
