/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/PrintVisitor.h>
#include <antares/expressions/visitors/VariadicNodeFunctionVisit.h>

namespace Antares::Expressions::Visitors
{
std::string PrintVisitor::visit(const Nodes::SumNode* node)
{
    const auto& operands = node->getOperands();
    if (operands.empty())
    {
        return "()";
    }
    return std::accumulate(std::next(std::begin(operands)),
                           std::end(operands),
                           "(" + dispatch(operands[0]),
                           [this](std::string sum, Nodes::Node* operand)
                           { return sum + "+" + dispatch(operand); })
           + ")";
}

std::string PrintVisitor::visit(const Nodes::SubtractionNode* node)
{
    return "(" + dispatch(node->left()) + "-" + dispatch(node->right()) + ")";
}

std::string PrintVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return "(" + dispatch(node->left()) + "*" + dispatch(node->right()) + ")";
}

std::string PrintVisitor::visit(const Nodes::DivisionNode* node)
{
    return "(" + dispatch(node->left()) + "/" + dispatch(node->right()) + ")";
}

std::string PrintVisitor::visit(const Nodes::EqualNode* node)
{
    return dispatch(node->left()) + "==" + dispatch(node->right());
}

std::string PrintVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    return dispatch(node->left()) + "<=" + dispatch(node->right());
}

std::string PrintVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    return dispatch(node->left()) + ">=" + dispatch(node->right());
}

std::string PrintVisitor::visit(const Nodes::NegationNode* node)
{
    return "-(" + dispatch(node->child()) + ")";
}

std::string PrintVisitor::visit(const Nodes::ParameterNode* node)
{
    return node->value();
}

std::string PrintVisitor::visit(const Nodes::VariableNode* node)
{
    return node->value();
}

std::string PrintVisitor::visit(const Nodes::LiteralNode* node)
{
    return std::to_string(node->value());
}

std::string PrintVisitor::visit(const Nodes::PortFieldNode* node)
{
    return node->getPortName() + "." + node->getFieldName();
}

std::string PrintVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    return node->getPortName() + "." + node->getFieldName();
}

std::string PrintVisitor::trimAndFormat(const std::string& in)
{
    auto s = in;
    // Trim left (remove leading whitespace)
    auto it = std::ranges::find_if_not(s, [](unsigned char ch) { return std::isspace(ch); });
    s.erase(s.begin(), it);

    // Ensure it starts with '+' or '-'
    if (!s.empty() && (s.front() != '-' && s.front() != '+'))
    {
        s.insert(s.begin(), '+');
    }
    return s;
}

std::string PrintVisitor::visit(const Nodes::TimeShiftNode* node)
{
    auto formatedShift = trimAndFormat(dispatch(node->right()));
    return dispatch(node->left()) + "[ t " + formatedShift + " ]";
}

std::string PrintVisitor::visit(const Nodes::TimeIndexNode* node)
{
    return dispatch(node->left()) + "[ " + dispatch(node->right()) + " ]";
}

std::string PrintVisitor::visit(const Nodes::TimeSumNode* node)
{
    return "sum(t" + trimAndFormat(dispatch(node->from())) + " .. t"
           + trimAndFormat(dispatch(node->to())) + ", " + dispatch(node->expression()) + ")";
}

std::string PrintVisitor::visit(const Nodes::AllTimeSumNode* node)
{
    return "sum(" + dispatch(node->child()) + ")";
}

std::string PrintVisitor::handleReducedCost(const Nodes::FunctionNode* node)
{
    const auto* varIdNode = dynamic_cast<Nodes::VariableNode*>(node->getOperands().at(0));
    return "reduced_cost(" + varIdNode->value() + ")";
}

// TODO rename
std::string PrintVisitor::ProcessOtherFunction(const Nodes::FunctionNode* node)
{
    std::string ret;
    if (node->size() >= 2)
    {
        ret = node->typeToString() + "(";
    }
    else
    {
        throw std::invalid_argument(
          "variadic Function printing: node must have at least 2 children");
    }
    const auto children = variadicFunction(*this, node);
    for (size_t i = 0; i < children.size(); ++i)
    {
        ret += children[i];
        if (i != children.size() - 1)
        {
            ret += ", ";
        }
    }
    ret += ")";
    return ret;
}

std::string PrintVisitor::handlePow(const Nodes::FunctionNode* node)
{
    return dispatch(node->getOperands().front()) + "^(" + dispatch(node->getOperands().at(1)) + ")";
}

std::string PrintVisitor::handleDual(const Nodes::FunctionNode* node)
{
    const auto* cstrIdNode = dynamic_cast<Nodes::ParameterNode*>(node->getOperands().at(0));
    return "dual(" + cstrIdNode->value() + ")";
}

std::string PrintVisitor::visit(const Nodes::FunctionNode* node)
{
    switch (node->type())
    {
    case Nodes::FunctionNodeType::reduced_cost:
        return handleReducedCost(node);
    case Nodes::FunctionNodeType::dual:
        return handleDual(node);
    case Nodes::FunctionNodeType::max:
    case Nodes::FunctionNodeType::min:
        return ProcessOtherFunction(node);
    case Nodes::FunctionNodeType::pow:
        return handlePow(node);
    default:
        return "";
    }
}

std::string PrintVisitor::name() const
{
    return "PrintVisitor";
}
} // namespace Antares::Expressions::Visitors
