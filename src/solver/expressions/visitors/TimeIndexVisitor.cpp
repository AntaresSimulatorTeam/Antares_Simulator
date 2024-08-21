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

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/TimeIndexVisitor.h>

namespace Antares::Solver::Visitors
{

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::AddNode& add)
{
    return dispatch(*add.left()) | dispatch(*add.right());
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::SubtractionNode& sub)
{
    return dispatch(*sub.left()) | dispatch(*sub.right());
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::MultiplicationNode& mult)
{
    return dispatch(*mult.left()) | dispatch(*mult.right());
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::DivisionNode& div)
{
    return dispatch(*div.left()) | dispatch(*div.right());
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::EqualNode& equ)
{
    return dispatch(*equ.left()) | dispatch(*equ.right());
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::LessThanOrEqualNode& lt)
{
    return dispatch(*lt.left()) | dispatch(*lt.right());
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::GreaterThanOrEqualNode& gt)
{
    return dispatch(*gt.left()) | dispatch(*gt.right());
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::VariableNode& var)
{
    return context_.at(&var);
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::ParameterNode& param)
{
    return context_.at(&param);
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::LiteralNode& lit)
{
    return TIME_STRUCTURE_TYPE::CONSTANT;
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::NegationNode& neg)
{
    return dispatch(*neg.child());
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(const Nodes::PortFieldNode& port_field_node)
{
    return context_.at(&port_field_node);
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(
  const Nodes::ComponentVariableNode& component_variable_node)
{
    return context_.at(&component_variable_node);
}

TIME_STRUCTURE_TYPE TimeIndexVisitor::visit(
  const Nodes::ComponentParameterNode& component_parameter_node)
{
    return context_.at(&component_parameter_node);
}

TimeIndexVisitor::TimeIndexVisitor(
  std::unordered_map<const Nodes::Node*, TIME_STRUCTURE_TYPE> context):
    context_(std::move(context))
{
}

} // namespace Antares::Solver::Visitors
