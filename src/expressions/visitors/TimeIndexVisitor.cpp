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

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/TimeIndexVisitor.h>

namespace Antares::Expressions::Visitors
{

TimeIndex TimeIndexVisitor::visit(const Nodes::SumNode* node)
{
    auto operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO,
                           [this](TimeIndex sum, Nodes::Node* operand)
                           { return sum | dispatch(operand); });
}

TimeIndex TimeIndexVisitor::visit(const Nodes::SubtractionNode* sub)
{
    return dispatch(sub->left()) | dispatch(sub->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::MultiplicationNode* mult)
{
    return dispatch(mult->left()) | dispatch(mult->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::DivisionNode* div)
{
    return dispatch(div->left()) | dispatch(div->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::EqualNode* equ)
{
    return dispatch(equ->left()) | dispatch(equ->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::LessThanOrEqualNode* lt)
{
    return dispatch(lt->left()) | dispatch(lt->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::GreaterThanOrEqualNode* gt)
{
    return dispatch(gt->left()) | dispatch(gt->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::VariableNode* var)
{
    return var->timeIndex();
}

TimeIndex TimeIndexVisitor::visit(const Nodes::ParameterNode* param)
{
    return param->timeIndex();
}

TimeIndex TimeIndexVisitor::visit([[maybe_unused]] const Nodes::LiteralNode* lit)
{
    return TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
}

TimeIndex TimeIndexVisitor::visit(const Nodes::NegationNode* neg)
{
    return dispatch(neg->child());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::PortFieldNode* port_field_node)
{
    // TODO FIXME
    return context_.at(port_field_node);
}

TimeIndex TimeIndexVisitor::visit(const Nodes::PortFieldSumNode* port_field_node)
{
    // TODO FIXME
    return context_.at(port_field_node);
}

TimeIndex TimeIndexVisitor::visit(const Nodes::ComponentVariableNode* component_variable_node)
{
    // TODO FIXME
    return context_.at(component_variable_node);
}

TimeIndex TimeIndexVisitor::visit(const Nodes::ComponentParameterNode* component_parameter_node)
{
    // TODO FIXME
    return context_.at(component_parameter_node);
}

TimeIndexVisitor::TimeIndexVisitor(std::unordered_map<const Nodes::Node*, TimeIndex> context):
    context_(std::move(context))
{
}

std::string TimeIndexVisitor::name() const
{
    return "TimeIndexVisitor";
}

} // namespace Antares::Expressions::Visitors
