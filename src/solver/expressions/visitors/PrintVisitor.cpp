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

namespace Antares::Solver::Visitors
{
std::string PrintVisitor::visit(const Nodes::AddNode& add)
{
    // Ici le compilateur (g++) a besoin de savoir qu'on veut le visit du type de base
    // sinon erreur de compil 'fonction non trouvée'
    return "(" + dispatch(*add[0]) + "+" + dispatch(*add[1]) + ")";
}

std::string PrintVisitor::visit(const Nodes::SubtractionNode& sub)
{
    return "(" + dispatch(*sub[0]) + "-" + dispatch(*sub[1]) + ")";
}

std::string PrintVisitor::visit(const Nodes::MultiplicationNode& mult)
{
    return "(" + dispatch(*mult[0]) + "*" + dispatch(*mult[1]) + ")";
}

std::string PrintVisitor::visit(const Nodes::DivisionNode& div)
{
    return "(" + dispatch(*div[0]) + "/" + dispatch(*div[1]) + ")";
}

std::string PrintVisitor::visit(const Nodes::EqualNode& equ)
{
    return dispatch(*equ[0]) + "==" + dispatch(*equ[1]);
}

std::string PrintVisitor::visit(const Nodes::LessThanOrEqualNode& lt)
{
    return dispatch(*lt[0]) + "<=" + dispatch(*lt[1]);
}

std::string PrintVisitor::visit(const Nodes::GreaterThanOrEqualNode& gt)
{
    return dispatch(*gt[0]) + ">=" + dispatch(*gt[1]);
}

std::string PrintVisitor::visit(const Nodes::NegationNode& neg)
{
    return "-(" + dispatch(*neg[0]) + ")";
}

std::string PrintVisitor::visit(const Nodes::ParameterNode& param)
{
    return param.getValue();
}

std::string PrintVisitor::visit(const Nodes::VariableNode& variable)
{
    return variable.getValue();
}

std::string PrintVisitor::visit(const Nodes::LiteralNode& lit)
{
    return std::to_string(lit.getValue());
}

std::string PrintVisitor::visit(const Nodes::PortFieldNode& port_field_node)
{
    return port_field_node.getPortName() + "." + port_field_node.getFieldName();
}

std::string PrintVisitor::visit(const Nodes::ComponentVariableNode& component_variable_node)
{
    return component_variable_node.getComponentId() + "."
           + component_variable_node.getComponentName();
}

std::string PrintVisitor::visit(const ComponentParameterNode& component_parameter_node)
{
    return component_parameter_node.getComponentId() + "."
           + component_parameter_node.getComponentName();
}
} // namespace Antares::Solver::Visitors
