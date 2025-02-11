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
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>

using namespace Antares::Expressions::Nodes;

namespace Antares::Optimization
{

ReadLinearExpressionVisitor::ReadLinearExpressionVisitor(
  Expressions::Visitors::EvaluationContext context):
    context_(std::move(context))
{
}

std::string ReadLinearExpressionVisitor::name() const
{
    return "ReadLinearExpressionVisitor";
}

LinearExpression ReadLinearExpressionVisitor::visit(const SumNode* node)
{
    auto operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           LinearExpression(),
                           [this](LinearExpression sum, Node* operand)
                           { return sum + dispatch(operand); });
}

LinearExpression ReadLinearExpressionVisitor::visit(const SubtractionNode* node)
{
    return dispatch(node->left()) - dispatch(node->right());
}

LinearExpression ReadLinearExpressionVisitor::visit(const MultiplicationNode* node)
{
    return dispatch(node->left()) * dispatch(node->right());
}

LinearExpression ReadLinearExpressionVisitor::visit(const DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

LinearExpression ReadLinearExpressionVisitor::visit(const EqualNode* node)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpression ReadLinearExpressionVisitor::visit(const LessThanOrEqualNode* node)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpression ReadLinearExpressionVisitor::visit(const GreaterThanOrEqualNode* node)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpression ReadLinearExpressionVisitor::visit(const NegationNode* node)
{
    return dispatch(node->child()).negate();
}

LinearExpression ReadLinearExpressionVisitor::visit(const VariableNode* node)
{
    return LinearExpression(0, {{node->value(), 1}});
}

LinearExpression ReadLinearExpressionVisitor::visit(const ParameterNode* node)
{
    return {context_.getParameterValue(node->value()), {}};
}

LinearExpression ReadLinearExpressionVisitor::visit(const LiteralNode* node)
{
    return {node->value(), {}};
}

LinearExpression ReadLinearExpressionVisitor::visit(const PortFieldNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

LinearExpression ReadLinearExpressionVisitor::visit(const PortFieldSumNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit PortFieldSumNodes");
}

LinearExpression ReadLinearExpressionVisitor::visit(const ComponentVariableNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentVariableNodes");
}

LinearExpression ReadLinearExpressionVisitor::visit(const ComponentParameterNode* node)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentParameterNodes");
}
} // namespace Antares::Optimization
