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

#include "antares/solver/expressions/visitors/AstGraphVisitor.h"

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

namespace Antares::Solver::Visitors
{

void AstGraphVisitor::visit(const Nodes::SumNode* node, std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id, "+", {"aqua", "hexagon", "filled, solid"}, os);
    for (auto child: node->getOperands())
    {
        int childId = getNodeID(child);
        os << "  " << id << " -> " << childId << ";\n";
        dispatch(child, os);
    }
}

void AstGraphVisitor::visit(const Nodes::SubtractionNode* node, std::ostream& os)
{
    processBinaryOperation(node, "-", {"moccasin", "oval", "filled, rounded"}, os);
}

void AstGraphVisitor::visit(const Nodes::MultiplicationNode* node, std::ostream& os)
{
    processBinaryOperation(node, "*", {"moccasin", "oval", "filled, rounded"}, os);
}

void AstGraphVisitor::visit(const Nodes::DivisionNode* node, std::ostream& os)
{
    processBinaryOperation(node, "/", {"moccasin", "oval", "filled, rounded"}, os);
}

void AstGraphVisitor::visit(const Nodes::EqualNode* node, std::ostream& os)
{
    processBinaryOperation(node, "==", {"beige", "oval", "filled, rounded"}, os);
}

void AstGraphVisitor::visit(const Nodes::LessThanOrEqualNode* node, std::ostream& os)
{
    processBinaryOperation(node, "<=", {"beige", "oval", "filled, rounded"}, os);
}

void AstGraphVisitor::visit(const Nodes::GreaterThanOrEqualNode* node, std::ostream& os)
{
    processBinaryOperation(node, ">=", {"beige", "oval", "filled, rounded"}, os);
}

void AstGraphVisitor::visit(const Nodes::VariableNode* node, std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id, "Var(" + node->value() + ")", {"gold", "box", "filled, solid"}, os);
}

void AstGraphVisitor::visit(const Nodes::ParameterNode* node, std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id, "Param(" + node->value() + ")", {"palegreen", "box", "filled, solid"}, os);
}

void AstGraphVisitor::visit(const Nodes::LiteralNode* node, std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id, std::to_string(node->value()), {"lightcyan", "box", "filled, solid"}, os);
}

void AstGraphVisitor::visit(const Nodes::NegationNode* node, std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id, "-", {"navajowhite", "square", "filled, solid"}, os);
    int childId = getNodeID(node->child());
    os << "  " << id << " -> " << childId << ";\n";
    dispatch(node->child(), os);
}

void AstGraphVisitor::visit(const Nodes::PortFieldNode* node, std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id,
             "PF(" + node->getPortName() + "," + node->getFieldName() + ")",
             {"powderblue", "invtriangle", "filled, solid"},
             os);
}

void AstGraphVisitor::visit(const Nodes::ComponentVariableNode* node, std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id,
             "CV(" + node->getComponentId() + "," + node->getComponentName() + ")",
             {"goldenrod", "box", "filled, solid"},
             os);
}

void AstGraphVisitor::visit(const Nodes::ComponentParameterNode* node, std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id,
             "CP(" + node->getComponentId() + "," + node->getComponentName() + ")",
             {"palegreen", "box", "filled, solid"},
             os);
}

std::string AstGraphVisitor::name() const
{
    return "AstGraphVisitor";
}

int AstGraphVisitor::getNodeID(const Nodes::Node* node)
{
    if (nodeIds_.find(node) == nodeIds_.end())
    {
        nodeIds_[node] = ++nodeCount_;
    }
    return nodeIds_[node];
}

void AstGraphVisitor::emitNode(int id,
                               const std::string& label,
                               const BoxStyle& box_style,
                               std::ostream& os)
{
    os << "  " << id << " [label=\"" << label << "\", shape=\"" << box_style.shape << "\", style=\""
       << box_style.style << "\", color=\"" << box_style.color << "\"];\n";
}

// Process binary operation nodes like Add, Subtract, etc.
void AstGraphVisitor::processBinaryOperation(const Nodes::BinaryNode* node,
                                             const std::string& label,
                                             const BoxStyle& box_style,
                                             std::ostream& os)
{
    int id = getNodeID(node);
    emitNode(id, label, box_style, os);

    const Nodes::Node* left = node->left();
    const Nodes::Node* right = node->right();

    int leftId = getNodeID(left);
    int rightId = getNodeID(right);

    os << "  " << id << " -> " << leftId << ";\n";
    os << "  " << id << " -> " << rightId << ";\n";

    dispatch(left, os);
    dispatch(right, os);
}

void AstGraphVisitor::NewTreeGraph(std::ostream& os, const std::string& tree_name)
{
    os << "digraph " + tree_name + " {\n";
    os << "node[style = filled]\n";
}

void AstGraphVisitor::EndTreeGraph(std::ostream& os)
{
    os << "}\n";
}

std::ostream& operator<<(std::ostream& os,
                         const std::pair<AstGraphVisitor&, Nodes::Node*>& visitorExpr)
{
    auto& [visitor, root] = visitorExpr;
    visitor.NewTreeGraph(os);
    visitor.dispatch(root, os);
    visitor.EndTreeGraph(os);
    return os;
}
} // namespace Antares::Solver::Visitors
