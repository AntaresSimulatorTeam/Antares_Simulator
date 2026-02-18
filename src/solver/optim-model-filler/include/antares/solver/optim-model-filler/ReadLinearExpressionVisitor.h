// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/study/system-model/component.h"

/**
 * Read Linear Expression Visitor
 * Visits a Node and produces a Linear Expression (defined by an offset and non-zero
 * coefficients of variables)
 * Comparison Nodes are not allowed
 */

using namespace Antares::Expressions;

namespace Antares::Optimisation
{

class ReadLinearExpressionVisitor: public Expressions::Visitors::NodeVisitor<
                                     Antares::Optimization::TimeDependentLinearExpression>
{
public:
    /**
     * @brief Constructs a clone visitor with the specified registry for creating new nodes.
     *
     * @param registry The registry used for creating new nodes.
     */
    explicit ReadLinearExpressionVisitor(
      const OptimEntityContainer& optimEntityContainer,
      const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
      const Antares::ModelerStudy::SystemModel::Component& component,
      const Optimisation::LinearProblemApi::ILinearProblemData* data,
      const Optimisation::ScenarioGroupRepository& scenarioGroupRepo);

    Antares::Optimization::TimeDependentLinearExpression visitMergeDuplicates(
      const Nodes::Node* node);

    std::string name() const override;

    Antares::Optimization::TimeDependentLinearExpression visit(const Nodes::SumNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::SubtractionNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::MultiplicationNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::DivisionNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::EqualNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::LessThanOrEqualNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::GreaterThanOrEqualNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::NegationNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::VariableNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::ParameterNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::LiteralNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::PortFieldNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::PortFieldSumNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::TimeShiftNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::TimeIndexNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::TimeSumNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::AllTimeSumNode* node) override;

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::FunctionNode* node) override;

private:
    Optimization::TimeDependentLinearExpression visitPower(const Nodes::FunctionNode* node);
    Optimization::TimeDependentLinearExpression visitFloor(const Nodes::FunctionNode* node);
    Optimization::TimeDependentLinearExpression visitCeil(const Nodes::FunctionNode* node);

    const Antares::Optimisation::OptimEntityContainer& optimEntityContainer_;
    const Antares::ModelerStudy::SystemModel::Component& component_;
    const Optimisation::ScenarioGroupRepository& scenarioGroupRepo_;
    const Optimisation::LinearProblemApi::IScenario* scenario_;
    const Antares::Optimisation::EvaluationContext evalContext_;
    const Antares::Optimisation::LinearProblemApi::FillContext& fillContext_;
    Antares::Expressions::Visitors::EvalVisitor evalVisitor_;
    const Optimisation::LinearProblemApi::ILinearProblemData* data_;

    const unsigned nbtimeSteps_;
};
} // namespace Antares::Optimisation
