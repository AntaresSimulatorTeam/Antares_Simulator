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

namespace Antares::Solver::Expressions
{
CloneVisitor::CloneVisitor(Registry<Node>& mem):
    registry_(mem)
{
}

Node* CloneVisitor::visit(const AddNode& input_node)
{
    return registry_.create<AddNode>(dispatch(*input_node[0]), dispatch(*input_node[1]));
}

Node* CloneVisitor::visit(const SubtractionNode& input_node)
{
    return registry_.create<SubtractionNode>(dispatch(*input_node[0]), dispatch(*input_node[1]));
}

Node* CloneVisitor::visit(const MultiplicationNode& input_node)
{
    return registry_.create<MultiplicationNode>(dispatch(*input_node[0]), dispatch(*input_node[1]));
}

Node* CloneVisitor::visit(const DivisionNode& input_node)
{
    return registry_.create<DivisionNode>(dispatch(*input_node[0]), dispatch(*input_node[1]));
}

Node* CloneVisitor::visit(const EqualNode& input_node)
{
    return registry_.create<EqualNode>(dispatch(*input_node[0]), dispatch(*input_node[1]));
}

Node* CloneVisitor::visit(const LessThanOrEqualNode& input_node)
{
    return registry_.create<LessThanOrEqualNode>(dispatch(*input_node[0]),
                                                 dispatch(*input_node[1]));
}

Node* CloneVisitor::visit(const GreaterThanOrEqualNode& input_node)
{
    return registry_.create<GreaterThanOrEqualNode>(dispatch(*input_node[0]),
                                                    dispatch(*input_node[1]));
}

Node* CloneVisitor::visit(const NegationNode& neg)
{
    return registry_.create<NegationNode>(dispatch(*neg[0]));
}

Node* CloneVisitor::visit(const VariableNode& param)
{
    return registry_.create<VariableNode>(param.getValue());
}

Node* CloneVisitor::visit(const ParameterNode& param)
{
    return registry_.create<ParameterNode>(param.getValue());
}

Node* CloneVisitor::visit(const LiteralNode& param)
{
    return registry_.create<LiteralNode>(param.getValue());
}

Node* CloneVisitor::visit(const PortFieldNode& port_field_node)
{
    return registry_.create<PortFieldNode>(port_field_node.port_name_, port_field_node.field_name_);
}

Node* CloneVisitor::visit(const ComponentVariableNode& component_variable_node)
{
    return registry_.create<ComponentVariableNode>(component_variable_node.component_id_,
                                                   component_variable_node.component_name_);
}

Node* CloneVisitor::visit(const ComponentParameterNode& component_parameter_node)
{
    return registry_.create<ComponentParameterNode>(component_parameter_node.component_id_,
                                                    component_parameter_node.component_name_);
}
} // namespace Antares::Solver::Expressions
