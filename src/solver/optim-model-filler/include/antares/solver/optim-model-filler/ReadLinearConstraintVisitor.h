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

#pragma once

#include <limits>

#include <antares/solver/expressions/visitors/EvaluationContext.h>
#include <antares/solver/expressions/visitors/NodeVisitor.h>

#include "ReadLinearExpressionVisitor.h"

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
 * - a Linear Expression (defined by an offset and non-zero variable coefficients)
 * - a lower and an upper bounds
 */
struct LinearConstraint
{
    std::map<std::string, double> coef_per_var;
    double lb = -std::numeric_limits<double>::infinity();
    double ub = std::numeric_limits<double>::infinity();
};

class ReadLinearConstraintVisitor
    : public Solver::Visitors::NodeVisitor<std::vector<LinearConstraint>>
{
public:
    ReadLinearConstraintVisitor() = default;
    explicit ReadLinearConstraintVisitor(
      Solver::Visitors::EvaluationContext context,
      const Solver::Modeler::Api::DataSeriesKeys& dataSeriesKeys);
    std::string name() const override;

private:
    ReadLinearExpressionVisitor linear_expression_visitor_;
    std::vector<LinearConstraint> visit(const Solver::Nodes::SumNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::SubtractionNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::MultiplicationNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::DivisionNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::EqualNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::LessThanOrEqualNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::GreaterThanOrEqualNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::NegationNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::VariableNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::ParameterNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::LiteralNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::PortFieldNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::PortFieldSumNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::ComponentVariableNode* node) override;
    std::vector<LinearConstraint> visit(const Solver::Nodes::ComponentParameterNode* node) override;
};
} // namespace Antares::Optimization
