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

#include <antares/solver/expressions/visitors/EvaluationContext.h>
#include <antares/solver/expressions/visitors/NodeVisitor.h>
#include <antares/solver/optim-model-filler/LinearExpression.h>

using namespace Antares::Solver::Visitors;
using namespace Antares::Solver::Nodes;

/**
 * Read Linear Expression Visitor
 * Visits a Node and produces a Linear Expression (defined by an offset and non-zero
 * coefficients of variables)
 * Comparison Nodes are not allowed
 */
namespace Antares::Optimization
{

class ReadLinearExpressionVisitor: public NodeVisitor<LinearExpression>
{
public:
    ReadLinearExpressionVisitor() = default;
    explicit ReadLinearExpressionVisitor(EvaluationContext context);
    std::string name() const override;

private:
    const EvaluationContext context_;
    LinearExpression visit(const SumNode* node) override;
    LinearExpression visit(const SubtractionNode* node) override;
    LinearExpression visit(const MultiplicationNode* node) override;
    LinearExpression visit(const DivisionNode* node) override;
    LinearExpression visit(const EqualNode* node) override;
    LinearExpression visit(const LessThanOrEqualNode* node) override;
    LinearExpression visit(const GreaterThanOrEqualNode* node) override;
    LinearExpression visit(const NegationNode* node) override;
    LinearExpression visit(const VariableNode* node) override;
    LinearExpression visit(const ParameterNode* node) override;
    LinearExpression visit(const LiteralNode* node) override;
    LinearExpression visit(const PortFieldNode* node) override;
    LinearExpression visit(const PortFieldSumNode* node) override;
    LinearExpression visit(const ComponentVariableNode* node) override;
    LinearExpression visit(const ComponentParameterNode* node) override;
};
} // namespace Antares::Optimization
