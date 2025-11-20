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
      const Antares::ModelerStudy::SystemModel::Component& component);

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
      const Nodes::ReducedCostNode* node) override;
    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::DualNode* node) override;

private:
    const Antares::Optimisation::OptimEntityContainer& optimEntityContainer_;
    const Antares::ModelerStudy::SystemModel::Component& component_;
    const Antares::Optimisation::EvaluationContext& evalContext_;
    const Antares::Optimisation::LinearProblemApi::FillContext& fillContext_;
    Antares::Expressions::Visitors::EvalVisitor evalVisitor_;
    const int nbtimeSteps_;
};
} // namespace Antares::Optimisation
