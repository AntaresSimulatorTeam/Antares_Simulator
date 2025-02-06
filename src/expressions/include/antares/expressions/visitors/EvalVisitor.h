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
#include "antares/expressions/visitors/NodeVisitor.h"

namespace Antares::Expressions::Visitors
{

class EvalVisitorDivisionException: public std::runtime_error
{
public:
    EvalVisitorDivisionException(double left, double right, const std::string& message);
};

class EvalVisitorNotImplemented: public std::invalid_argument
{
public:
    EvalVisitorNotImplemented(const std::string& visitor, const std::string& node);
};

/**
 * @brief Represents a visitor for evaluating expressions within a given context.
 */
class EvalVisitor: public NodeVisitor<double>
{
public:
    /**
     * @brief Default constructor, creates an evaluation visitor with no context.
     */
    EvalVisitor() = default; // No context (variables / parameters)

    /**
     * @brief Constructs an evaluation visitor with the specified context.
     *
     * @param context The evaluation context.
     */
    explicit EvalVisitor(EvaluationContext context);
    std::string name() const override;

private:
    const EvaluationContext context_;
    double visit(const Nodes::SumNode* node) override;
    double visit(const Nodes::SubtractionNode* node) override;
    double visit(const Nodes::MultiplicationNode* node) override;
    double visit(const Nodes::DivisionNode* node) override;
    double visit(const Nodes::EqualNode* node) override;
    double visit(const Nodes::LessThanOrEqualNode* node) override;
    double visit(const Nodes::GreaterThanOrEqualNode* node) override;
    double visit(const Nodes::NegationNode* node) override;
    double visit(const Nodes::VariableNode* node) override;
    double visit(const Nodes::ParameterNode* node) override;
    double visit(const Nodes::LiteralNode* node) override;
    double visit(const Nodes::PortFieldNode* node) override;
    double visit(const Nodes::PortFieldSumNode* node) override;
    double visit(const Nodes::ComponentVariableNode* node) override;
    double visit(const Nodes::ComponentParameterNode* node) override;
};
} // namespace Antares::Expressions::Visitors
