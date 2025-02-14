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
#include <iostream>
#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/PrintVisitor.h>

namespace Antares::Expressions::Visitors
{
std::string PrintVisitor::visit(const Nodes::SumNode* node)
{
    auto operands = node->getOperands();
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

std::string PrintVisitor::visit(const Nodes::ComponentVariableNode* node)
{
    return node->getComponentId() + "." + node->getComponentName();
}

std::string PrintVisitor::visit(const Nodes::ComponentParameterNode* node)
{
    return node->getComponentId() + "." + node->getComponentName();
}

std::string PrintVisitor::name() const
{
    return "PrintVisitor";
}
} // namespace Antares::Expressions::Visitors
