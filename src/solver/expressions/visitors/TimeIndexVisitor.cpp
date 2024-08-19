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

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::AddNode& add)
{
    return dispatch(*add.left()) + dispatch(*add.right());
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::SubtractionNode& sub)
{
    return dispatch(*sub.left()) - dispatch(*sub.right());
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::MultiplicationNode& mult)
{
    return dispatch(*mult.left()) * dispatch(*mult.right());
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::DivisionNode& div)
{
    return dispatch(*div.left()) / dispatch(*div.right());
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::EqualNode& equ)
{
    return dispatch(*equ.left()).Connect(dispatch(*equ.right()));
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::LessThanOrEqualNode& lt)
{
    return dispatch(*lt.left()).Connect(dispatch(*lt.right()));
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::GreaterThanOrEqualNode& gt)
{
    return dispatch(*gt.left()).Connect(dispatch(*gt.right()));
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::VariableNode& var)
{
    return var.getTimeType();
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::ParameterNode& param)
{
    return param.getTimeType();
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::LiteralNode& lit)
{
    return Nodes::TimeType::CONSTANT;
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::NegationNode& neg)
{
    return -dispatch(*neg.child());
}

ExpressionTimeType TimeIndexVisitor::visit(const Nodes::PortFieldNode& port_field_node)
{
    // TODO must be resolved from the context
    return Nodes::TimeType::CONSTANT;
}

ExpressionTimeType TimeIndexVisitor::visit(
  const Nodes::ComponentVariableNode& component_variable_node)
{
    // TODO must be resolved
    return Nodes::TimeType::CONSTANT;
}

ExpressionTimeType TimeIndexVisitor::visit(
  const Nodes::ComponentParameterNode& component_parameter_node)
{
    // TODO must be resolved
    return Nodes::TimeType::CONSTANT;
}

} // namespace Antares::Solver::Visitors
