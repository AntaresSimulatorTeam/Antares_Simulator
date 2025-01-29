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
CloneVisitor::CloneVisitor(Registry<Nodes::Node>& registry):
    registry_(registry)
{
}

Nodes::Node* CloneVisitor::visit(const Nodes::SumNode* node)
{
    std::vector<Nodes::Node*> clonedOperands;
    clonedOperands.reserve(node->size());
    for (auto* operand: node->getOperands())
    {
        clonedOperands.push_back(dispatch(operand));
    }
    // Give ownership of clonedOperands to the caller
    return registry_.create<Nodes::SumNode>(std::move(clonedOperands));
}

Nodes::Node* CloneVisitor::visit(const Nodes::SubtractionNode* node)
{
    return registry_.create<Nodes::SubtractionNode>(dispatch(node->left()),
                                                    dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return registry_.create<Nodes::MultiplicationNode>(dispatch(node->left()),
                                                       dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::DivisionNode* node)
{
    return registry_.create<Nodes::DivisionNode>(dispatch(node->left()), dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::EqualNode* node)
{
    return registry_.create<Nodes::EqualNode>(dispatch(node->left()), dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    return registry_.create<Nodes::LessThanOrEqualNode>(dispatch(node->left()),
                                                        dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    return registry_.create<Nodes::GreaterThanOrEqualNode>(dispatch(node->left()),
                                                           dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::NegationNode* negationNode)
{
    return registry_.create<Nodes::NegationNode>(dispatch(negationNode->child()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::VariableNode* variableNode)
{
    return registry_.create<Nodes::VariableNode>(variableNode->value(), variableNode->timeIndex());
}

Nodes::Node* CloneVisitor::visit(const Nodes::ParameterNode* parameterNode)
{
    return registry_.create<Nodes::ParameterNode>(parameterNode->value());
}

Nodes::Node* CloneVisitor::visit(const Nodes::LiteralNode* literalNode)
{
    return registry_.create<Nodes::LiteralNode>(literalNode->value());
}

Nodes::Node* CloneVisitor::visit(const Nodes::PortFieldNode* portfieldNode)
{
    return registry_.create<Nodes::PortFieldNode>(portfieldNode->getPortName(),
                                                  portfieldNode->getFieldName());
}

Nodes::Node* CloneVisitor::visit(const Nodes::PortFieldSumNode* portfieldSumNode)
{
    return registry_.create<Nodes::PortFieldSumNode>(portfieldSumNode->getPortName(),
                                                     portfieldSumNode->getFieldName());
}

Nodes::Node* CloneVisitor::visit(const Nodes::ComponentVariableNode* component_variable_node)
{
    return registry_.create<Nodes::ComponentVariableNode>(
      component_variable_node->getComponentId(),
      component_variable_node->getComponentName());
}

Nodes::Node* CloneVisitor::visit(const Nodes::ComponentParameterNode* component_parameter_node)
{
    return registry_.create<Nodes::ComponentParameterNode>(
      component_parameter_node->getComponentId(),
      component_parameter_node->getComponentName());
}

std::string CloneVisitor::name() const
{
    return "CloneVisitor";
}

} // namespace Antares::Solver::Visitors
