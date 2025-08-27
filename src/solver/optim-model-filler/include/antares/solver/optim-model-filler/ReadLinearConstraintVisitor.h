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

#pragma once

#include <limits>
#include <ranges>
#include <utility>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/solver/optim-model-filler/LinearExpression.h>
#include "antares/expressions/ShiftVector.h"

#include "ReadLinearExpressionVisitor.h"

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;

/**
 * Read Linear Constraint Visitor
 * Visits a Node and produces a Linear Constraint (defined by its Linear Expression and bounds).
 * The root node is expected to be a comparison node.
 */
namespace Antares::Optimization
{

/**
 * Linear Constraint
 * Represents a linear constraint in an optimization problem.
 * It is fully defined by:
 * - a Linear Expression (defined by an offset and non-zero variable coefficients by index)
 * - a lower and an upper bounds
 */
struct LinearConstraint
{
    VarIndexMap coef_per_var; // Migré vers VarIndexMap pour accès O(1)
    double lb = -std::numeric_limits<double>::infinity();
    double ub = std::numeric_limits<double>::infinity();
    unsigned int timeStep = 0;
};

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
class ReadLinearConstraintVisitor
    : public Expressions::Visitors::NodeVisitor<std::vector<LinearConstraint>>
{
public:
    ReadLinearConstraintVisitor() = default;
    explicit ReadLinearConstraintVisitor(
      Expressions::Visitors::EvaluationContext context,
      const Optimisation::LinearProblemApi::FillContext& fillContext,
      const Antares::ModelerStudy::SystemModel::Component& component,
      const VariableDictionary<InnerSolverVariable>& variable_dictionary);

    std::string name() const override;

private:
    std::vector<LinearConstraint> visit(const Expressions::Nodes::SumNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::SubtractionNode* node) override;
    std::vector<LinearConstraint> visit(
      const Expressions::Nodes::MultiplicationNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::DivisionNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::EqualNode* node) override;
    std::vector<LinearConstraint> visit(
      const Expressions::Nodes::LessThanOrEqualNode* node) override;
    std::vector<LinearConstraint> visit(
      const Expressions::Nodes::GreaterThanOrEqualNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::NegationNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::VariableNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::ParameterNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::LiteralNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::PortFieldNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::PortFieldSumNode* node) override;
    std::vector<LinearConstraint> visit(
      const Expressions::Nodes::ComponentVariableNode* node) override;
    std::vector<LinearConstraint> visit(
      const Expressions::Nodes::ComponentParameterNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::TimeShiftNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::TimeIndexNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::TimeSumNode* node) override;
    std::vector<LinearConstraint> visit(const Expressions::Nodes::AllTimeSumNode* node) override;

    ReadLinearExpressionVisitor<InnerSolverVariable> linear_expression_visitor_;
};

// Implémentation template

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
ReadLinearConstraintVisitor<InnerSolverVariable>::ReadLinearConstraintVisitor(
  Expressions::Visitors::EvaluationContext context,
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  const Antares::ModelerStudy::SystemModel::Component& component,
  const VariableDictionary<InnerSolverVariable>& variable_dictionary):
    linear_expression_visitor_(std::move(context), fillContext, component, variable_dictionary)
{
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::string ReadLinearConstraintVisitor<InnerSolverVariable>::name() const
{
    return "ReadLinearConstraintVisitor";
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const EqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());

    const auto& leftMinusRightLinearExpression = leftMinusRight.GetLinearExpressions();
    std::vector<LinearConstraint> constraints;
    constraints.reserve(leftMinusRightLinearExpression.size());

    for (const auto& [timeStep, value]: leftMinusRightLinearExpression)
    {
        constraints.emplace_back(LinearConstraint{.coef_per_var = value.coefPerIndex(),
                                                  .lb = -value.offset(),
                                                  .ub = -value.offset(),
                                                  .timeStep = timeStep});
    }
    return constraints;
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const LessThanOrEqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());

    const auto& leftMinusRightLinearExpression = leftMinusRight.GetLinearExpressions();
    std::vector<LinearConstraint> constraints;
    constraints.reserve(leftMinusRightLinearExpression.size());

    for (const auto& [timeStep, value]: leftMinusRightLinearExpression)
    {
        constraints.emplace_back(LinearConstraint{.coef_per_var = value.coefPerIndex(),
                                                  .ub = -value.offset(),
                                                  .timeStep = timeStep});
    }
    return constraints;
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const GreaterThanOrEqualNode* node)
{
    auto leftMinusRight = linear_expression_visitor_.dispatch(node->left())
                          - linear_expression_visitor_.dispatch(node->right());

    const auto& leftMinusRightLinearExpression = leftMinusRight.GetLinearExpressions();
    std::vector<LinearConstraint> constraints;
    constraints.reserve(leftMinusRightLinearExpression.size());

    for (const auto& [timeStep, value]: leftMinusRightLinearExpression)
    {
        constraints.emplace_back(LinearConstraint{.coef_per_var = value.coefPerIndex(),
                                                  .lb = -value.offset(),
                                                  .timeStep = timeStep});
    }
    return constraints;
}

namespace
{
std::invalid_argument IllegalNodeException()
{
    return std::invalid_argument("Root node of a constraint must be a comparator.");
}
} // namespace

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const SumNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const SubtractionNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const MultiplicationNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const DivisionNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const NegationNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const VariableNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const ParameterNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const LiteralNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const PortFieldNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const PortFieldSumNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const ComponentVariableNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const ComponentParameterNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const TimeShiftNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const TimeIndexNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const TimeSumNode*)
{
    throw IllegalNodeException();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::vector<LinearConstraint> ReadLinearConstraintVisitor<InnerSolverVariable>::visit(
  const AllTimeSumNode*)
{
    throw IllegalNodeException();
}

} // namespace Antares::Optimization
