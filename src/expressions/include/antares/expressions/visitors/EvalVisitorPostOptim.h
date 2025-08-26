/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/study/system-model/component.h"

namespace Antares::Expressions::Visitors
{

/**
 * @brief Represents a visitor for evaluating expressions within a given context.
 */
class EvalVisitorPostOptim: EvalVisitor
{
public:
    /**
     * @brief Constructs an evaluation visitor with the specified context.
     *
     * @param context The evaluation context.
     * @param fillContext
     */
    explicit EvalVisitorPostOptim(EvaluationContext context,
                                  Optimisation::LinearProblemApi::FillContext fillContext,
                                  const ModelerStudy::SystemModel::Component* component);

    std::string name() const override;

    EvaluationResult visit(const Nodes::SumNode* node) override;
    EvaluationResult visit(const Nodes::SubtractionNode* node) override;
    EvaluationResult visit(const Nodes::MultiplicationNode* node) override;
    EvaluationResult visit(const Nodes::DivisionNode* node) override;
    EvaluationResult visit(const Nodes::EqualNode* node) override;
    EvaluationResult visit(const Nodes::LessThanOrEqualNode* node) override;
    EvaluationResult visit(const Nodes::GreaterThanOrEqualNode* node) override;
    EvaluationResult visit(const Nodes::NegationNode* node) override;
    EvaluationResult visit(const Nodes::VariableNode* node) override;
    EvaluationResult visit(const Nodes::ParameterNode* node) override;
    EvaluationResult visit(const Nodes::LiteralNode* node) override;
    EvaluationResult visit(const Nodes::PortFieldNode* node) override;
    EvaluationResult visit(const Nodes::PortFieldSumNode* node) override;
    EvaluationResult visit(const Nodes::ComponentVariableNode* node) override;
    EvaluationResult visit(const Nodes::ComponentParameterNode* node) override;
    EvaluationResult visit(const Nodes::TimeShiftNode* node) override;
    EvaluationResult visit(const Nodes::TimeIndexNode* node) override;
    EvaluationResult visit(const Nodes::TimeSumNode* node) override;
    EvaluationResult visit(const Nodes::AllTimeSumNode* node) override;
};
} // namespace Antares::Expressions::Visitors
