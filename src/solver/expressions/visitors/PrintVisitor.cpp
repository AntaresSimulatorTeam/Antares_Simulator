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

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/PrintVisitor.h>

namespace Antares::Solver::Expressions
{
std::string PrintVisitor::visit(const AddNode& add)
{
    // Ici le compilateur (g++) a besoin de savoir qu'on veut le visit du type de base
    // sinon erreur de compil 'fonction non trouvée'
    return dispatch(*add[0]) + "+" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const SubtractionNode& add)
{
    return dispatch(*add[0]) + "-" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const MultiplicationNode& add)
{
    return dispatch(*add[0]) + "*" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const DivisionNode& add)
{
    return dispatch(*add[0]) + "/" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const EqualNode& add)
{
    return dispatch(*add[0]) + "==" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const LessThanNode& add)
{
    return dispatch(*add[0]) + "<" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const LessThanOrEqualNode& add)
{
    return dispatch(*add[0]) + "<=" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const GreaterThanNode& add)
{
    return dispatch(*add[0]) + ">" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const GreaterThanOrEqualNode& add)
{
    return dispatch(*add[0]) + ">=" + dispatch(*add[1]);
}

std::string PrintVisitor::visit(const NegationNode& neg)
{
    return "-(" + dispatch(*neg[0]) + ")";
}

std::string PrintVisitor::visit(const ParameterNode& param)
{
    return param.getValue();
}

std::string PrintVisitor::visit(const VariableNode& variable)
{
    return variable.getValue();
}

std::string PrintVisitor::visit(const LiteralNode& lit)
{
    return std::to_string(lit.getValue());
}

std::string PrintVisitor::visit(const PortFieldNode& port_field_node)
{
    return port_field_node.port_name_ + "." + port_field_node.field_name_;
}

std::string PrintVisitor::visit(const ComponentVariableNode& component_variable_node)
{
    return component_variable_node.component_id_ + "." + component_variable_node.component_name_;
}

std::string PrintVisitor::visit(const ComponentParameterNode& component_parameter_node)
{
    return component_parameter_node.component_id_ + "." + component_parameter_node.component_name_;
}
} // namespace Antares::Solver::Expressions
