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

#include <ranges>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>

using namespace Antares::Expressions::Nodes;

namespace Antares::Optimization
{

ReadLinearConstraintVisitor::ReadLinearConstraintVisitor(
  Expressions::Visitors::EvaluationContext context,
  const Optimisation::LinearProblemApi::FillContext& fillContext):
    linear_expression_visitor_(std::move(context), fillContext)
{
}

std::string ReadLinearConstraintVisitor::name() const
{
    return "ReadLinearConstraintVisitor";
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const EqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());

    const auto& leftMinusRightLinearExpression = leftMinusRight.GetLinearExpressions();
    std::vector<LinearConstraint> constraints;
    constraints.reserve(leftMinusRightLinearExpression.size());

    for (const auto& [timeStep, value]: leftMinusRightLinearExpression)
    {
        constraints.emplace_back(LinearConstraint{.coef_per_var = value.coefPerVar(),
                                                  .lb = -value.offset(),
                                                  .ub = -value.offset(),
                                                  .timeStep = timeStep});
    }
    return constraints;
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const LessThanOrEqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());

    const auto& leftMinusRightLinearExpression = leftMinusRight.GetLinearExpressions();
    std::vector<LinearConstraint> constraints;
    constraints.reserve(leftMinusRightLinearExpression.size());

    for (const auto& [timeStep, value]: leftMinusRightLinearExpression)
    {
        constraints.emplace_back(LinearConstraint{.coef_per_var = value.coefPerVar(),
                                                  .ub = -value.offset(),
                                                  .timeStep = timeStep});
    }
    return constraints;
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const GreaterThanOrEqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());

    const auto& leftMinusRightLinearExpression = leftMinusRight.GetLinearExpressions();
    std::vector<LinearConstraint> constraints;
    constraints.reserve(leftMinusRightLinearExpression.size());

    for (const auto& [timeStep, value]: leftMinusRightLinearExpression)
    {
        constraints.emplace_back(LinearConstraint{.coef_per_var = value.coefPerVar(),
                                                  .lb = -value.offset(),
                                                  .timeStep = timeStep});
    }
    return constraints;
}

static std::invalid_argument IllegalNodeException()
{
    return std::invalid_argument("Root node of a constraint must be a comparator.");
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const SumNode* sum_node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const SubtractionNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const MultiplicationNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const DivisionNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const NegationNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const VariableNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const ParameterNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const LiteralNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const PortFieldNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const PortFieldSumNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const ComponentVariableNode* node)
{
    throw IllegalNodeException();
}

std::vector<LinearConstraint> ReadLinearConstraintVisitor::visit(const ComponentParameterNode* node)
{
    throw IllegalNodeException();
}
} // namespace Antares::Optimization
