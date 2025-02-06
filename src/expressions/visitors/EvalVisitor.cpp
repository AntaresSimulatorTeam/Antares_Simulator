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

#include "antares/expressions/visitors/EvalVisitor.h"

#include <cmath>
#include <numeric>
#include <ranges>

#include "yuni/io/directory/info/platform.h"

#include <antares/expressions/nodes/ExpressionsNodes.h>

namespace Antares::Expressions::Visitors
{
EvalVisitor::EvalVisitor(EvaluationContext context):
    context_(std::move(context))
{
}

std::vector<double> EvalVisitor::visit(const Nodes::SumNode* node)
{
    // auto operands = node->getOperands();
    // return std::accumulate(std::begin(operands),
    //                        std::end(operands),
    //                        0,
    //                        [this](double sum, Nodes::Node* operand)
    //                        { return sum + dispatch(operand); });

    const auto& operands = node->getOperands();
    if (operands.empty())
    {
        return {};
    }

    std::vector<double> result = dispatch(operands.front());

    for (size_t i = 1; i < operands.size(); ++i)
    {
        const auto& operandVec = dispatch(operands[i]);
        if (operandVec.size() != result.size())
        {
            throw std::runtime_error("Vector size mismatch in SumNode");
        }
        std::ranges::transform(result, operandVec, result.begin(), std::plus<>());
    }

    return result;
}

std::vector<double> EvalVisitor::visit(const Nodes::SubtractionNode* node)
{
    // return dispatch(node->left()) - dispatch(node->right());

    auto left = dispatch(node->left());
    auto right = dispatch(node->right());

    if (left.size() != right.size())
    {
        throw std::runtime_error("Vector size mismatch in SubtractionNode");
    }

    std::vector<double> result(left.size());
    std::ranges::transform(left, right, result.begin(), std::minus<>());

    return result;
}

std::vector<double> EvalVisitor::visit(const Nodes::MultiplicationNode* node)
{
    // return dispatch(node->left()) * dispatch(node->right());

    auto left = dispatch(node->left());
    auto right = dispatch(node->right());

    if (left.size() != right.size())
    {
        throw std::runtime_error("Vector size mismatch in MultiplicationNode");
    }

    std::vector<double> result(left.size());
    std::ranges::transform(left, right, result.begin(), std::multiplies<>());

    return result;
}

std::vector<double> EvalVisitor::visit(const Nodes::DivisionNode* node)
{
    const auto left = dispatch(node->left());
    const auto right = dispatch(node->right());
    std::vector<double> result(left.size());
    unsigned int index = 0;
    try
    {
        for (; index < left.size(); ++index)
        {
            result[index] = left[index] / right[index];
            if (!std::isfinite(result[index]))
            {
                throw EvalVisitorDivisionException(left[index],
                                                   right[index],
                                                   "is not a finite number");
            }
        }
    }
    catch (const std::exception& ex)
    {
        throw EvalVisitorDivisionException(left[index], right[index], ex.what());
    }
    return result;
}

std::vector<double> EvalVisitor::visit(const Nodes::EqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

std::vector<double> EvalVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

std::vector<double> EvalVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

std::vector<double> EvalVisitor::visit(const Nodes::VariableNode* node)
{
    // TODO where it's used? // Exception?
    return std::vector<double>(context_.numberOfTimesteps(),
                               context_.getVariableValue(node->value()));
}

std::vector<double> EvalVisitor::visit(const Nodes::ParameterNode* node)
{
    return context_.getParameterValue(node->value());
}

std::vector<double> EvalVisitor::visit(const Nodes::LiteralNode* node)
{
    return std::vector<double>(context_.numberOfTimesteps(), node->value());
}

std::vector<double> EvalVisitor::visit(const Nodes::NegationNode* node)
{
    auto ret = dispatch(node->child());
    std::ranges::transform(ret, ret.begin(), [](double x) { return x * -1; });
    return ret;
}

std::vector<double> EvalVisitor::visit(const Nodes::PortFieldNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

std::vector<double> EvalVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

std::vector<double> EvalVisitor::visit(const Nodes::ComponentVariableNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

std::vector<double> EvalVisitor::visit(const Nodes::ComponentParameterNode* node)
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
} // namespace Antares::Expressions::Visitors
