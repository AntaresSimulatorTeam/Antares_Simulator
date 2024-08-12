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

namespace Antares::Solver::Expressions
{
EvalVisitor::EvalVisitor(EvaluationContext context):
    context_(std::move(context))
{
}

double EvalVisitor::visit(const AddNode& add)
{
    return dispatch(*add[0]) + dispatch(*add[1]);
}

double EvalVisitor::visit(const SubtractionNode& add)
{
    return dispatch(*add[0]) - dispatch(*add[1]);
}

double EvalVisitor::visit(const MultiplicationNode& add)
{
    return dispatch(*add[0]) * dispatch(*add[1]);
}

double EvalVisitor::visit(const DivisionNode& add)
{
    if (auto divisor = dispatch(*add[1]); divisor != 0)
    {
        return dispatch(*add[0]) / divisor;
    }
    else
    {
        throw EvalVisitorDivisionException("DivisionNode Divide by zero");
    }
}

double EvalVisitor::visit(const EqualNode& add)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const LessThanOrEqualNode& add)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const GreaterThanOrEqualNode& add)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const VariableNode& param)
{
    return context_.getVariableValue(param.getValue());
}

double EvalVisitor::visit(const ParameterNode& param)
{
    return context_.getParameterValue(param.getValue());
}

double EvalVisitor::visit(const LiteralNode& lit)
{
    return lit.getValue();
}

double EvalVisitor::visit(const NegationNode& neg)
{
    return -dispatch(*neg[0]);
}

double EvalVisitor::visit(const PortFieldNode& port_field_node)
{
    return 0.;
}

double EvalVisitor::visit(const ComponentVariableNode& component_variable_node)
{
    return 0.;
}

double EvalVisitor::visit(const ComponentParameterNode& component_parameter_node)
{
    // TODO
    return 0.;
}
} // namespace Antares::Solver::Expressions
