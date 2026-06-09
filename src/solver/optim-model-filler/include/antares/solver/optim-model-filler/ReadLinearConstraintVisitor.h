// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/visitors/NodeVisitor.h>

#include "ReadLinearExpressionVisitor.h"
#include "TimeDependentLinearExpression.h"

namespace Antares::Optimisation
{
class OptimEntityContainer;
} // namespace Antares::Optimisation

/**
 * Read Linear Constraint Visitor
 * Visits a Node and produces a Linear Constraint (defined by its Linear Expression and bounds).
 * The root node is expected to be a comparison node.
 */
namespace Antares::Optimisation
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
    Antares::Optimization::TimeDependentLinearExpression coef_per_var;
    std::vector<double> lb;
    std::vector<double> ub;
};

class ReadLinearConstraintVisitor final: public Expressions::Visitors::NodeVisitor<LinearConstraint>
{
public:
    ReadLinearConstraintVisitor() = delete;
    explicit ReadLinearConstraintVisitor(
      const Optimisation::OptimEntityContainer& optimEntityContainer,
      const Optimisation::LinearProblemApi::FillContext& fillContext,
      const ModelerStudy::SystemModel::Component& component,
      const Optimisation::LinearProblemApi::ILinearProblemData* data,
      const Optimisation::ScenarioGroupRepository& scenarioGroupRepository);

    std::string name() const override;

private:
    LinearConstraint visit(const Expressions::Nodes::SumNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::SubtractionNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::MultiplicationNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::DivisionNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::EqualNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::LessThanOrEqualNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::GreaterThanOrEqualNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::NegationNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::VariableNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::ParameterNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::LiteralNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::PortFieldNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::PortFieldSumNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::TimeShiftNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::TimeIndexNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::TimeSumNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::TPlusNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::AllTimeSumNode* node) override;
    LinearConstraint visit(const Expressions::Nodes::FunctionNode* node) override;

    ReadLinearExpressionVisitor linear_expression_visitor_;
};
} // namespace Antares::Optimisation
