// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <ranges>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include "antares/exception/InvalidArgumentError.hpp"

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Optimisation
{

ReadLinearConstraintVisitor::ReadLinearConstraintVisitor(
  const Optimisation::OptimEntityContainer& optimEntityContainer,
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  const Component& component,
  const LinearProblemApi::ILinearProblemData* data,
  const ScenarioGroupRepository& scenarioGroupRepo):
    linear_expression_visitor_(optimEntityContainer,
                               fillContext,
                               component,
                               data,
                               scenarioGroupRepo)
{
}

std::string ReadLinearConstraintVisitor::name() const
{
    return "ReadLinearConstraintVisitor";
}

std::vector<double> operator-(const std::vector<double>& in)
{
    std::vector<double> ret(in);
    for (double& x: ret)
    {
        x = -x;
    }
    return ret;
}

LinearConstraint ReadLinearConstraintVisitor::visit(const EqualNode* node)
{
    auto left = linear_expression_visitor_.dispatch(node->left());
    left -= linear_expression_visitor_.dispatch(node->right());
    left.mergeDuplicateCoefficients();
    const std::vector<double> constant = -left.constant();
    return {.coef_per_var = left, .lb = constant, .ub = constant};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const LessThanOrEqualNode* node)
{
    auto left = linear_expression_visitor_.dispatch(node->left());
    left -= linear_expression_visitor_.dispatch(node->right()); // TODO
    left.mergeDuplicateCoefficients();
    const std::vector<double> constant = left.constant();

    return {.coef_per_var = left,
            .lb = std::vector<double>(left.size(), -std::numeric_limits<double>::infinity()),
            .ub = -constant};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const GreaterThanOrEqualNode* node)
{
    auto left = linear_expression_visitor_.dispatch(node->left());

    left -= linear_expression_visitor_.dispatch(node->right());
    left.mergeDuplicateCoefficients();
    return {.coef_per_var = left,
            .lb = -left.constant(),
            .ub = std::vector<double>(left.size(), std::numeric_limits<double>::infinity())};
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

LinearConstraint ReadLinearConstraintVisitor::visit(const TPlusNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const AllTimeSumNode*)
{
    throw IllegalNodeException();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const FunctionNode*)
{
    throw IllegalNodeException();
}
} // namespace Antares::Optimisation
