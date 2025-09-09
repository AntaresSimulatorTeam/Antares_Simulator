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

#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/FullKey.h>
#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/study/system-model/component.h"

#include "EvaluationContextProvider.h"

/**
 * Read Linear Expression Visitor
 * Visits a Node and produces a Linear Expression (defined by an offset and non-zero
 * coefficients of variables)
 * Comparison Nodes are not allowed
 */
namespace Antares::Optimization
{

class ReadLinearExpressionVisitor
    : public Expressions::Visitors::NodeVisitor<TimeDependentLinearExpression>
{
public:
    ReadLinearExpressionVisitor() = delete;
    ReadLinearExpressionVisitor(const Optimisation::EvaluationContextProvider& evalContextProvider,
                                const Optimisation::LinearProblemApi::FillContext& fillContext,
                                const ModelerStudy::SystemModel::Component& component);
    std::string name() const override;

private:
    TimeDependentLinearExpression visit(const Expressions::Nodes::SumNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::SubtractionNode* node) override;
    TimeDependentLinearExpression visit(
      const Expressions::Nodes::MultiplicationNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::DivisionNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::EqualNode* node) override;
    TimeDependentLinearExpression visit(
      const Expressions::Nodes::LessThanOrEqualNode* node) override;
    TimeDependentLinearExpression visit(
      const Expressions::Nodes::GreaterThanOrEqualNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::NegationNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::VariableNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::ParameterNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::LiteralNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::PortFieldNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::PortFieldSumNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::TimeShiftNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::TimeIndexNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::TimeSumNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::AllTimeSumNode* node) override;

    const Optimisation::EvaluationContextProvider& evalContextProvider_;
    const Expressions::Visitors::EvaluationContext evalContext_;
    const Optimisation::LinearProblemApi::FillContext& fillContext_;
    const ModelerStudy::SystemModel::Component& component_;
    Expressions::Visitors::EvalVisitor evalVisitor_;
};
} // namespace Antares::Optimization
