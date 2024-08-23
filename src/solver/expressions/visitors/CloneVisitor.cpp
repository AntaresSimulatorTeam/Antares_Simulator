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
#include <antares/solver/expressions/visitors/CloneVisitor.h>

namespace Antares::Solver::Visitors
{
CloneVisitor::CloneVisitor(Registry<Nodes::Node>& mem):
    registry_(mem)
{
}

Nodes::Node* CloneVisitor::visit(const Nodes::AddNode& node)
{
    return registry_.create<Nodes::AddNode>(dispatch(*node.left()), dispatch(*node.right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::SubtractionNode& node)
{
    return registry_.create<Nodes::SubtractionNode>(dispatch(*node.left()),
                                                    dispatch(*node.right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::MultiplicationNode& node)
{
    return registry_.create<Nodes::MultiplicationNode>(dispatch(*node.left()),
                                                       dispatch(*node.right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::DivisionNode& node)
{
    return registry_.create<Nodes::DivisionNode>(dispatch(*node.left()), dispatch(*node.right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::EqualNode& node)
{
    return registry_.create<Nodes::EqualNode>(dispatch(*node.left()), dispatch(*node.right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::LessThanOrEqualNode& node)
{
    return registry_.create<Nodes::LessThanOrEqualNode>(dispatch(*node.left()),
                                                        dispatch(*node.right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::GreaterThanOrEqualNode& node)
{
    return registry_.create<Nodes::GreaterThanOrEqualNode>(dispatch(*node.left()),
                                                           dispatch(*node.right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::NegationNode& neg)
{
    return registry_.create<Nodes::NegationNode>(dispatch(*neg.child()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::VariableNode& param)
{
    return registry_.create<Nodes::VariableNode>(param.getValue());
}

Nodes::Node* CloneVisitor::visit(const Nodes::ParameterNode& param)
{
    return registry_.create<Nodes::ParameterNode>(param.getValue());
}

Nodes::Node* CloneVisitor::visit(const Nodes::LiteralNode& param)
{
    return registry_.create<Nodes::LiteralNode>(param.getValue());
}

Nodes::Node* CloneVisitor::visit(const Nodes::PortFieldNode& port_field_node)
{
    return registry_.create<Nodes::PortFieldNode>(port_field_node.getPortName(),
                                                  port_field_node.getFieldName());
}

Nodes::Node* CloneVisitor::visit(const Nodes::ComponentVariableNode& component_variable_node)
{
    return registry_.create<Nodes::ComponentVariableNode>(
      component_variable_node.getComponentId(),
      component_variable_node.getComponentName());
}

Nodes::Node* CloneVisitor::visit(const Nodes::ComponentParameterNode& component_parameter_node)
{
    return registry_.create<Nodes::ComponentParameterNode>(
      component_parameter_node.getComponentId(),
      component_parameter_node.getComponentName());
}

std::string CloneVisitor::name() const
{
    return "CloneVisitor";
}

} // namespace Antares::Solver::Visitors
