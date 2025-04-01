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

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include "antares/study/system-model/component.h"
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::Study;

namespace Antares::Optimization
{

ReadLinearExpressionVisitor::ReadLinearExpressionVisitor(
  EvaluationContext evalContext,
  Optimisation::LinearProblemApi::FillContext fillContext,
  const std::string& componentId,
  const std::vector<SystemModel::Connection>& connections):
    evalContext_(std::move(evalContext)),
    fillContext_(std::move(fillContext)),
    componentId_(componentId),
    connections_(connections),
    evalVisitor_(evalContext_, fillContext_)
{
}

std::string ReadLinearExpressionVisitor::name() const
{
    return "ReadLinearExpressionVisitor";
}

std::vector<const SystemModel::Component*> ReadLinearExpressionVisitor::getConnectedComponents()
{
    std::vector<const SystemModel::Component*> connectedComponents;
    for (auto& c: connections_)
    {
        if (c.firstEntry().component()->Id() == componentId_)
        {
            connectedComponents.push_back(c.secondEntry().component());
        }
        if (c.secondEntry().component()->Id() == componentId_)
        {
            connectedComponents.push_back(c.firstEntry().component());
        }
    }
    return connectedComponents;
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const SumNode* node)
{
    auto operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           TimeDependentLinearExpression(fillContext_),
                           [this](TimeDependentLinearExpression sum, Node* operand)
                           { return sum + dispatch(operand); });
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const SubtractionNode* node)
{
    return dispatch(node->left()) - dispatch(node->right());
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const MultiplicationNode* node)
{
    return dispatch(node->left()) * dispatch(node->right());
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const EqualNode* node)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const LessThanOrEqualNode* node)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const GreaterThanOrEqualNode* node)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const NegationNode* node)
{
    return -dispatch(node->child());
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const VariableNode* node)
{
    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        return TimeDependentLinearExpression(
          fillContext_,
          LinearExpression(0, {{FullKey(componentId_, node->value()), 1}}));
    }
    else // only dependent
    {
        LinearExpressionMap linearExpressions;

        for (unsigned int timeStep = fillContext_.getFirstTimeStep();
             timeStep <= fillContext_.getLastTimeStep();
             ++timeStep)
        {
            linearExpressions[timeStep] = LinearExpression(
              0,
              {{FullKey(componentId_, node->value(), 0 /*TODO */, timeStep), 1}});
        }
        return TimeDependentLinearExpression(linearExpressions);
    }
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const ParameterNode* node)
{
    const auto systemParameter = evalContext_.getParameter(node->value());
    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        && systemParameter.type != ParameterType::CONSTANT)
    {
        throw std::invalid_argument(
          "Parameter " + node->value()
          + " is declared constant in time and scenario in library but not in system");
    }
    else if (systemParameter.type == ParameterType::CONSTANT)
    {
        return TimeDependentLinearExpression(
          fillContext_,
          LinearExpression(evalContext_.getSystemParameterValueAsDouble(node->value()), {}));
    }
    else // only dependent
    {
        LinearExpressionMap linearExpressions;

        for (auto timeStep = fillContext_.getFirstTimeStep();
             timeStep <= fillContext_.getLastTimeStep();
             ++timeStep)
        {
            linearExpressions[timeStep] = LinearExpression(
              evalContext_.getParameterValue(node->value(), "", 0, timeStep),
              {});
        }
        return TimeDependentLinearExpression(linearExpressions);
    }
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const LiteralNode* node)
{
    return TimeDependentLinearExpression(fillContext_, LinearExpression(node->value(), {}));
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const PortFieldNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const PortFieldSumNode* node)
{
    std::string port = node->getPortName();
    std::string field = node->getFieldName();

    auto connectedComponents = getConnectedComponents();

    TimeDependentLinearExpression portFieldSum(fillContext_, {});
    for (const auto* c: connectedComponents)
    {
        const EvaluationContext connectedComponentEvalContext(c->getParameterValues(),
                                                              {},
                                                              evalContext_.data());
        ReadLinearExpressionVisitor visitor(connectedComponentEvalContext,
                                            fillContext_,
                                            c->Id(),
                                            {});
        
        const PortFieldNode* portFieldNode = c->getPortFieldNode(port + "." + field);
        portFieldSum += visitor.dispatch(portFieldNode);
    }

    return portFieldSum;
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const ComponentVariableNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentVariableNodes");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const ComponentParameterNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentParameterNodes");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const TimeShiftNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeShift = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return ret.shiftLinearExpressions(timeShift);
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const TimeIndexNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeIndex = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return ret[timeIndex];
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const TimeSumNode* node)
{
    const auto expression = dispatch(node->expression());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto from = static_cast<int>(evalVisitor_.dispatch(node->from()).valueAsDouble());

    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto to = static_cast<int>(evalVisitor_.dispatch(node->to()).valueAsDouble());

    return expression.timeSumLinearExpressions(from, to);
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const AllTimeSumNode* node)
{
    const auto expression = dispatch(node->child());
    return expression.allTimeSumLinearExpressions();
}
} // namespace Antares::Optimization
