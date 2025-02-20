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
using namespace Antares::Expressions::Nodes;

namespace Antares::Optimization
{

ReadLinearExpressionVisitor::ReadLinearExpressionVisitor(
  Expressions::Visitors::EvaluationContext context,
  Optimisation::LinearProblemApi::FillContext fillContext):
    context_(std::move(context)),
    fillContext_(std::move(fillContext))
{
}

std::string ReadLinearExpressionVisitor::name() const
{
    return "ReadLinearExpressionVisitor";
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
    return TimeDependentLinearExpression(fillContext_, LinearExpression(0, {{node->value(), 1}}));
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const ParameterNode* node)
{
    const auto systemParameter = context_.getParameter(node->value());
    if (node->timeIndex() == Expressions::Visitors::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        && systemParameter.type != Expressions::Visitors::ParameterType::CONSTANT)
    {
        throw std::invalid_argument(
          "Parameter " + node->value()
          + " is declared constant in time and scenario in library but not in system");
    }
    else if (systemParameter.type == Expressions::Visitors::ParameterType::CONSTANT)
    {
        return TimeDependentLinearExpression(
          fillContext_,
          LinearExpression(context_.getSystemParameterValueAsDouble(node->value()), {}));
    }
    else // only timedepent
    {
        std::map<unsigned int, LinearExpression> linearExpressions;

        for (auto timeStep = fillContext_.getFirstTimeStep();
             timeStep <= fillContext_.getLastTimeStep();
             ++timeStep)
        {
            linearExpressions[timeStep] = LinearExpression(
              context_.getParameterValue(node->value(), "", 0, timeStep),
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
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit PortFieldSumNodes");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const ComponentVariableNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentVariableNodes");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const ComponentParameterNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentParameterNodes");
}
} // namespace Antares::Optimization
