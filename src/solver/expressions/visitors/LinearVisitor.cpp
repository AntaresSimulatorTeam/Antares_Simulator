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

#include "antares/solver/expressions/visitors/LinearVisitor.h"

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{

bool LinearVisitor::visit(const AddNode& add)
{
    return dispatch(*add[0]) && dispatch(*add[1]);
}

bool LinearVisitor::visit(const SubtractionNode& sub)
{
    return dispatch(*sub[0]) && dispatch(*sub[1]);
}

bool LinearVisitor::visit(const MultiplicationNode& mult)
{
    // TODO The product of a constant and a linear expression is linear
    return dispatch(*mult[0]) && dispatch(*mult[1]);
}

bool LinearVisitor::visit(const DivisionNode& div)
{
    return false;
}

bool LinearVisitor::visit(const EqualNode& equ)
{
    // not implemented for comparison node
    return 0.;
}

bool LinearVisitor::visit(const LessThanOrEqualNode& lt)
{
    // not implemented for comparison node
    return 0.;
}

bool LinearVisitor::visit(const GreaterThanOrEqualNode& gt)
{
    // not implemented for comparison node
    return 0.;
}

bool LinearVisitor::visit(const VariableNode& var)
{
    return true;
}

bool LinearVisitor::visit(const ParameterNode& param)
{
    return true;
}

bool LinearVisitor::visit(const LiteralNode& lit)
{
    return true;
}

bool LinearVisitor::visit(const NegationNode& neg)
{
    return dispatch(*neg[0]);
}

bool LinearVisitor::visit(const PortFieldNode& port_field_node)
{
    // TODO
    return true;
}

bool LinearVisitor::visit(const ComponentVariableNode& component_variable_node)
{
    // TODO
    return true;
}

bool LinearVisitor::visit(const ComponentParameterNode& component_parameter_node)
{
    // TODO
    return true;
}
} // namespace Antares::Solver::Expressions
