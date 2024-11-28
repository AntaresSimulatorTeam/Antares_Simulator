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
#include "antares/solver/expressions/visitors/NodeVisitor.h"

using namespace Antares::Solver::Visitors;
using namespace Antares::Solver::Nodes;

// TODO : doc
namespace Antares::Optimization
{

struct LinearConstraint
{
    enum Sign
    {
        LEQ,
        GEQ,
        EQ
    };

    double scalar_value = 0;
    std::map<std::string, double> coef_per_var;
    Sign sign;
};

class ReadLinearConstraintVisitor: public NodeVisitor<LinearConstraint>
{
public:
    /**
     * @brief Default constructor, creates an evaluation visitor with no context. //TODO
     */
    ReadLinearConstraintVisitor() = default; // No context (variables / parameters) //TODO

    /**
     * @brief Constructs an evaluation visitor with the specified context. //TODO
     *
     * @param context The evaluation context.
     */
    explicit ReadLinearConstraintVisitor(EvaluationContext context);
    std::string name() const override;

private:
    const EvaluationContext context_;
    LinearConstraint visit(const SumNode* node) override;
    LinearConstraint visit(const SubtractionNode* node) override;
    LinearConstraint visit(const MultiplicationNode* node) override;
    LinearConstraint visit(const DivisionNode* node) override;
    LinearConstraint visit(const EqualNode* node) override;
    LinearConstraint visit(const LessThanOrEqualNode* node) override;
    LinearConstraint visit(const GreaterThanOrEqualNode* node) override;
    LinearConstraint visit(const NegationNode* node) override;
    LinearConstraint visit(const VariableNode* node) override;
    LinearConstraint visit(const ParameterNode* node) override;
    LinearConstraint visit(const LiteralNode* node) override;
    LinearConstraint visit(const PortFieldNode* node) override;
    LinearConstraint visit(const PortFieldSumNode* node) override;
    LinearConstraint visit(const ComponentVariableNode* node) override;
    LinearConstraint visit(const ComponentParameterNode* node) override;
};
} // namespace Antares::Optimization
