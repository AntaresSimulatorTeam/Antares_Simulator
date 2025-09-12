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

#include <ranges>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/expressions/ShiftVector.h"

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Optimization
{

ReadLinearConstraintVisitor::ReadLinearConstraintVisitor(
  const Optimisation::EvaluationContextProvider& evalContextProvider,
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  const Component& component,
  unsigned int nbModelVariables,
  const std::vector<unsigned int>& variableStartColumn):
    linear_expression_visitor_(evalContextProvider,
                               fillContext,
                               component,
                               nbModelVariables,
                               variableStartColumn)
{
}

std::string ReadLinearConstraintVisitor::name() const
{
    return "ReadLinearConstraintVisitor";
}

LinearConstraint ReadLinearConstraintVisitor::visit(const EqualNode* node)
{
    auto left = linear_expression_visitor_.dispatch(node->left());
    left -= linear_expression_visitor_.dispatch(node->right());
    const auto boundary = -left.offset();
    return {.coef_per_var = left.coefPerVar(), .lb = boundary, .ub = boundary};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const LessThanOrEqualNode* node)
{
    auto left = linear_expression_visitor_.dispatch(node->left());
    left -= linear_expression_visitor_.dispatch(node->right());

    return {.coef_per_var = left.coefPerVar(),
            .lb = Eigen::VectorXd::Constant(left.offset().rows(),
                                            -std::numeric_limits<double>::infinity()),
            .ub = -left.offset()};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const GreaterThanOrEqualNode* node)
{
    auto left = linear_expression_visitor_.dispatch(node->left());
    left -= linear_expression_visitor_.dispatch(node->right());
    return {.coef_per_var = left.coefPerVar(),
            .lb = -left.offset(),
            .ub = Eigen::VectorXd::Constant(left.offset().rows(),
                                            std::numeric_limits<double>::infinity())};
}

static Error::InvalidArgumentError IllegalNodeException()
{
    return Error::InvalidArgumentError("Root node of a constraint must be a comparator.");
}

LinearConstraint ReadLinearConstraintVisitor::visit(const SumNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const SubtractionNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const MultiplicationNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const DivisionNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const NegationNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const VariableNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const ParameterNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const LiteralNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const PortFieldNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const PortFieldSumNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const TimeShiftNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const TimeIndexNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const TimeSumNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const AllTimeSumNode*)
{
    throw IllegalNodeException();
}
} // namespace Antares::Optimization
