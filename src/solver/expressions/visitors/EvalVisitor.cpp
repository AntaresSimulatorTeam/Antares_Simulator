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

#include "antares/solver/expressions/visitors/EvalVisitor.h"

#include <cmath>
#include <numeric>

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

namespace Antares::Solver::Visitors
{
EvalVisitor::EvalVisitor(EvaluationContext context):
    context_(std::move(context))
{
}

double EvalVisitor::visit(const Nodes::SumNode* node)
{
    auto operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           0,
                           [this](double sum, Nodes::Node* operand)
                           { return sum + dispatch(operand); });
}

double EvalVisitor::visit(const Nodes::SubtractionNode* node)
{
    return dispatch(node->left()) - dispatch(node->right());
}

double EvalVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return dispatch(node->left()) * dispatch(node->right());
}

double EvalVisitor::visit(const Nodes::DivisionNode* node)
{
    double left = dispatch(node->left());
    double right = dispatch(node->right());
    double result = 0.;
    try
    {
        result = left / right;
        if (!std::isfinite(result))
        {
            throw EvalVisitorDivisionException(left, right, "is not a finite number");
        }
    }
    catch (const std::exception& ex)
    {
        throw EvalVisitorDivisionException(left, right, ex.what());
    }
    return result;
}

double EvalVisitor::visit(const Nodes::EqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

double EvalVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

double EvalVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

double EvalVisitor::visit(const Nodes::VariableNode* node)
{
    return context_.getVariableValue(node->value());
}

double EvalVisitor::visit(const Nodes::ParameterNode* node)
{
    return context_.getParameterValue(node->value());
}

double EvalVisitor::visit(const Nodes::LiteralNode* node)
{
    return node->value();
}

double EvalVisitor::visit(const Nodes::NegationNode* node)
{
    return -dispatch(node->child());
}

double EvalVisitor::visit(const Nodes::PortFieldNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

double EvalVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

double EvalVisitor::visit(const Nodes::ComponentVariableNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

double EvalVisitor::visit(const Nodes::ComponentParameterNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

std::string EvalVisitor::name() const
{
    return "EvalVisitor";
}

EvalVisitorDivisionException::EvalVisitorDivisionException(double left,
                                                           double right,
                                                           const std::string& message):
    std::runtime_error("DivisionNode: Error while evaluating : " + std::to_string(left) + "/"
                       + std::to_string(right) + " " + message)
{
}

EvalVisitorNotImplemented::EvalVisitorNotImplemented(const std::string& visitor,
                                                     const std::string& node):
    std::invalid_argument("Visitor" + visitor + " not implemented for node type " + node)
{
}
} // namespace Antares::Solver::Visitors
