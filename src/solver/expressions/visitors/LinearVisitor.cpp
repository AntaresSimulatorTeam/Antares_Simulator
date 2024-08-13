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

namespace Antares::Solver::Visitors
{

LinearOperations LinearVisitor::visit(const Nodes::AddNode& add)
{
    return dispatch(*add[0]) + dispatch(*add[1]);
}

LinearOperations LinearVisitor::visit(const Nodes::SubtractionNode& sub)
{
    return dispatch(*sub[0]) - dispatch(*sub[1]);
}

LinearOperations LinearVisitor::visit(const Nodes::MultiplicationNode& mult)
{
    return dispatch(*mult[0]) * dispatch(*mult[1]);
}

LinearOperations LinearVisitor::visit(const Nodes::DivisionNode& div)
{
    return dispatch(*div[0]) / dispatch(*div[1]);
}

LinearOperations LinearVisitor::visit(const Nodes::EqualNode& equ)
{ // TODO
    return LinearOperations(LinearStatus::NON_LINEAR);
}

LinearOperations LinearVisitor::visit(const Nodes::LessThanOrEqualNode& lt)
{
    // TODO
    return LinearOperations(LinearStatus::NON_LINEAR);
}

LinearOperations LinearVisitor::visit(const Nodes::GreaterThanOrEqualNode& gt)
{
    // TODO
    return LinearOperations(LinearStatus::NON_LINEAR);
}

LinearOperations LinearVisitor::visit(const Nodes::VariableNode& var)
{
    // TODO
    //    return var.getValue() == variable_;
}

LinearOperations LinearVisitor::visit(const Nodes::ParameterNode& param)
{
    return true;
}

LinearOperations LinearVisitor::visit(const Nodes::LiteralNode& lit)
{
    return true;
}

LinearOperations LinearVisitor::visit(const Nodes::NegationNode& neg)
{
    return dispatch(*neg[0]);
}

LinearOperations LinearVisitor::visit(const Nodes::PortFieldNode& port_field_node)
{
    // TODO
    return true;
}

LinearOperations LinearVisitor::visit(const Nodes::ComponentVariableNode& component_variable_node)
{
    // TODO
    return true;
}

LinearOperations LinearVisitor::visit(const Nodes::ComponentParameterNode& component_parameter_node)
{
    // TODO
    return true;
}

LinearVisitor::LinearVisitor(const std::string variable):
    variable_(variable)
{
}

LinearOperations::LinearOperations(const LinearStatus& status):
    status_(status)
{
}

LinearOperations LinearOperations::operator*(const LinearOperations& other)
{
    switch (other.status_)
    {
    case LinearStatus::NON_LINEAR:
        return LinearOperations(LinearStatus::NON_LINEAR);
    case LinearStatus::CONSTANT:
        return LinearOperations(status_);
    case LinearStatus::LINEAR:
        if (status_ == LinearStatus::CONSTANT)
        {
            return LinearOperations(other.status_);
        }
        else
        {
            return LinearOperations(LinearStatus::NON_LINEAR);
        }
    };
}

LinearOperations LinearOperations::operator/(const LinearOperations& other)
{
    switch (other.status_)
    {
    case LinearStatus::NON_LINEAR:
    case LinearStatus::LINEAR:
        return LinearOperations(LinearStatus::NON_LINEAR);
    case LinearStatus::CONSTANT:
        return LinearOperations(status_);
    };
}

LinearStatus LinearOperations::getStatus() const
{
    return status_;
}

LinearOperations LinearOperations::operator+(const LinearOperations& other)
{
    switch (other.status_)
    {
    case LinearStatus::NON_LINEAR:
        return LinearOperations(LinearStatus::NON_LINEAR);
    case LinearStatus::CONSTANT:
        return LinearOperations(status_);
    case LinearStatus::LINEAR:
        if (status_ == LinearStatus::CONSTANT || status_ == LinearStatus::LINEAR)
        {
            return LinearOperations(other.status_);
        }
        else
        {
            return LinearOperations(LinearStatus::NON_LINEAR);
        }
    };
}

LinearOperations LinearOperations::operator-(const LinearOperations& other)
{
    return operator+(other);
}

} // namespace Antares::Solver::Visitors
