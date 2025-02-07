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

#include "antares/expressions/visitors/LinearityVisitor.h"

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/LinearStatus.h>

namespace Antares::Expressions::Visitors
{

LinearStatus LinearityVisitor::visit(const Nodes::SumNode* node)
{
    auto operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           LinearStatus::CONSTANT,
                           [this](LinearStatus sum, Nodes::Node* operand)
                           { return sum + dispatch(operand); });
}

LinearStatus LinearityVisitor::visit(const Nodes::SubtractionNode* node)
{
    return dispatch(node->left()) - dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return dispatch(node->left()) * dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::EqualNode* node)
{
    return dispatch(node->left()) + dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    return dispatch(node->left()) + dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    return dispatch(node->left()) + dispatch(node->right());
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::VariableNode*)
{
    return LinearStatus::LINEAR;
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::ParameterNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::LiteralNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::NegationNode* node)
{
    return -dispatch(node->child());
}

LinearStatus LinearityVisitor::visit(const Nodes::PortFieldNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::PortFieldSumNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::ComponentVariableNode*)
{
    return LinearStatus::LINEAR;
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::ComponentParameterNode*)
{
    return LinearStatus::CONSTANT;
}

std::string LinearityVisitor::name() const
{
    return "LinearityVisitor";
}
} // namespace Antares::Expressions::Visitors
