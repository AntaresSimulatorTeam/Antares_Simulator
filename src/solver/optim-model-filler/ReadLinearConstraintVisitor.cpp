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

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/LinearExpression.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>

using namespace Antares::Expressions::Nodes;

namespace Antares::Optimization
{

ReadLinearConstraintVisitor::ReadLinearConstraintVisitor(
  Expressions::Visitors::EvaluationContext context):
    linear_expression_visitor_(std::move(context))
{
}

std::string ReadLinearConstraintVisitor::name() const
{
    return "ReadLinearConstraintVisitor";
}

LinearConstraint ReadLinearConstraintVisitor::visit(const EqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());
    return LinearConstraint{.coef_per_var = leftMinusRight.coefPerVar(),
                            .lb = -leftMinusRight.offset(),
                            .ub = -leftMinusRight.offset()};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const LessThanOrEqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());
    return LinearConstraint{.coef_per_var = leftMinusRight.coefPerVar(),
                            .ub = -leftMinusRight.offset()};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const GreaterThanOrEqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());
    return LinearConstraint{.coef_per_var = leftMinusRight.coefPerVar(),
                            .lb = -leftMinusRight.offset()};
}

static std::invalid_argument IllegalNodeException()
{
    return std::invalid_argument("Root node of a constraint must be a comparator.");
}

LinearConstraint ReadLinearConstraintVisitor::visit(const SumNode* sum_node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const SubtractionNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const MultiplicationNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const DivisionNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const NegationNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const VariableNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const ParameterNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const LiteralNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const PortFieldNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const PortFieldSumNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const ComponentVariableNode* node)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const ComponentParameterNode* node)
{
    throw IllegalNodeException();
}
} // namespace Antares::Optimization
