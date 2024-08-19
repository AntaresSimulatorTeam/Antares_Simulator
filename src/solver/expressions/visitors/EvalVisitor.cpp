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

#include "antares/solver/expressions/visitors/EvalVisitor.h"

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

namespace Antares::Solver::Visitors
{
EvalVisitor::EvalVisitor(EvaluationContext context):
    context_(std::move(context))
{
}

double EvalVisitor::visit(const Nodes::AddNode& add)
{
    return dispatch(*add.left()) + dispatch(*add.right());
}

double EvalVisitor::visit(const Nodes::SubtractionNode& sub)
{
    return dispatch(*sub.left()) - dispatch(*sub.right());
}

double EvalVisitor::visit(const Nodes::MultiplicationNode& mult)
{
    return dispatch(*mult.left()) * dispatch(*mult.right());
}

double EvalVisitor::visit(const Nodes::DivisionNode& div)
{
    if (auto divisor = dispatch(*div.right()); divisor == 0.0)
    {
        throw EvalVisitorDivisionException("DivisionNode Division by zero");
    }
    else
    {
        return dispatch(*div.left()) / divisor;
    }
}

double EvalVisitor::visit(const Nodes::EqualNode& equ)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const Nodes::LessThanOrEqualNode& lt)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const Nodes::GreaterThanOrEqualNode& gt)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const Nodes::VariableNode& var)
{
    return context_.getVariableValue(var.getValue());
}

double EvalVisitor::visit(const Nodes::ParameterNode& param)
{
    return context_.getParameterValue(param.getValue());
}

double EvalVisitor::visit(const Nodes::LiteralNode& lit)
{
    return lit.getValue();
}

double EvalVisitor::visit(const Nodes::NegationNode& neg)
{
    return -dispatch(*neg.child());
}

double EvalVisitor::visit(const Nodes::PortFieldNode& port_field_node)
{
    return 0.;
}

double EvalVisitor::visit(const Nodes::ComponentVariableNode& component_variable_node)
{
    return 0.;
}

double EvalVisitor::visit(const Nodes::ComponentParameterNode& component_parameter_node)
{
    // TODO
    return 0.;
}
} // namespace Antares::Solver::Visitors
