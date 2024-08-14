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

#include "antares/solver/expressions/visitors/LinearityVisitor.h"

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/LinearStatus.h>

namespace Antares::Solver::Visitors
{

LinearStatus LinearityVisitor::visit(const Nodes::AddNode& add)
{
    return dispatch(*add[0]) + dispatch(*add[1]);
}

LinearStatus LinearityVisitor::visit(const Nodes::SubtractionNode& sub)
{
    return dispatch(*sub[0]) - dispatch(*sub[1]);
}

LinearStatus LinearityVisitor::visit(const Nodes::MultiplicationNode& mult)
{
    return dispatch(*mult[0]) * dispatch(*mult[1]);
}

LinearStatus LinearityVisitor::visit(const Nodes::DivisionNode& div)
{
    return dispatch(*div[0]) / dispatch(*div[1]);
}

LinearStatus LinearityVisitor::visit(const Nodes::EqualNode& equ)
{
    return LinearStatus::NON_LINEAR;
}

LinearStatus LinearityVisitor::visit(const Nodes::LessThanOrEqualNode& lt)
{
    return LinearStatus::NON_LINEAR;
}

LinearStatus LinearityVisitor::visit(const Nodes::GreaterThanOrEqualNode& gt)
{
    return LinearStatus::NON_LINEAR;
}

LinearStatus LinearityVisitor::visit(const Nodes::VariableNode& var)
{
    return LinearStatus::LINEAR;
}

LinearStatus LinearityVisitor::visit(const Nodes::ParameterNode& param)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::LiteralNode& lit)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::NegationNode& neg)
{
    return -dispatch(*neg.child());
}

LinearStatus LinearityVisitor::visit(const Nodes::PortFieldNode& port_field_node)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::ComponentVariableNode& component_variable_node)
{
    return LinearStatus::LINEAR;
}

LinearStatus LinearityVisitor::visit(const Nodes::ComponentParameterNode& component_parameter_node)
{
    return LinearStatus::CONSTANT;
}

} // namespace Antares::Solver::Visitors
