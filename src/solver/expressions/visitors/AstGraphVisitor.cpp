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

void AstGraphVisitor::visit(const Nodes::SumNode* node)
{
    int id = getNodeID(node);
    emitNode(id, "+", "aqua", "hexagon", "filled, solid");
    for (auto child: node->getOperands())
    {
        int childId = getNodeID(child);
        *out_stream_ << "  " << id << " -> " << childId << ";\n";
        dispatch(child);
    }
}

void AstGraphVisitor::visit(const Nodes::SubtractionNode* node)
{
    processBinaryOperation(node, "-", "moccasin", "oval", "filled, rounded");
}

void AstGraphVisitor::visit(const Nodes::MultiplicationNode* node)
{
    processBinaryOperation(node, "*", "moccasin", "oval", "filled, rounded");
}

void AstGraphVisitor::visit(const Nodes::DivisionNode* node)
{
    processBinaryOperation(node, "/", "moccasin", "oval", "filled, rounded");
}

void AstGraphVisitor::visit(const Nodes::EqualNode* node)
{
    processBinaryOperation(node, "==", "beige", "oval", "filled, rounded");
}

void AstGraphVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    processBinaryOperation(node, "<=", "beige", "oval", "filled, rounded");
}

void AstGraphVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    processBinaryOperation(node, ">=", "beige", "oval", "filled, rounded");
}

void AstGraphVisitor::visit(const Nodes::VariableNode* node)
{
    int id = getNodeID(node);
    emitNode(id, "Var(" + node->value() + ")", "gold", "box", "filled, solid");
}

void AstGraphVisitor::visit(const Nodes::ParameterNode* node)
{
    int id = getNodeID(node);
    emitNode(id, "Param(" + node->value() + ")", "palegreen", "box", "filled, solid");
}

void AstGraphVisitor::visit(const Nodes::LiteralNode* node)
{
    int id = getNodeID(node);
    emitNode(id, std::to_string(node->value()), "lightcyan", "box", "filled, solid");
}

void AstGraphVisitor::visit(const Nodes::NegationNode* node)
{
    int id = getNodeID(node);
    emitNode(id, "-", "navajowhite", "square", "filled, solid");
    int childId = getNodeID(node->child());
    *out_stream_ << "  " << id << " -> " << childId << ";\n";
    dispatch(node->child());
}

void AstGraphVisitor::visit(const Nodes::PortFieldNode* node)
{
    int id = getNodeID(node);
    emitNode(id,
             "PF(" + node->getPortName() + "," + node->getFieldName() + ")",
             "powderblue",
             "invtriangle",
             "filled, solid");
}

void AstGraphVisitor::visit(const Nodes::ComponentVariableNode* node)
{
    int id = getNodeID(node);
    emitNode(id,
             "CV(" + node->getComponentId() + "," + node->getComponentName() + ")",
             "goldenrod",
             "box",
             "filled, solid");
}

void AstGraphVisitor::visit(const Nodes::ComponentParameterNode* node)
{
    int id = getNodeID(node);
    emitNode(id,
             "CP(" + node->getComponentId() + "," + node->getComponentName() + ")",
             "palegreen",
             "box",
             "filled, solid");
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
                               const std::string& color,
                               const std::string& shape,
                               const std::string& style)
{
    *out_stream_ << "  " << id << " [label=\"" << label << "\", shape=\"" << shape << "\", style=\""
                 << style << "\", color=\"" << color << "\"];\n";
}

// Process binary operation nodes like Add, Subtract, etc.
void AstGraphVisitor::processBinaryOperation(const Nodes::BinaryNode* node,
                                             const std::string& label,
                                             const std::string& color,
                                             const std::string& shape,
                                             const std::string& style)
{
    int id = getNodeID(node);
    emitNode(id, label, color, shape, style);

    const Nodes::Node* left = node->left();
    const Nodes::Node* right = node->right();

    int leftId = getNodeID(left);
    int rightId = getNodeID(right);

    *out_stream_ << "  " << id << " -> " << leftId << ";\n";
    *out_stream_ << "  " << id << " -> " << rightId << ";\n";

    dispatch(left);
    dispatch(right);
}

AstGraphVisitor::AstGraphVisitor(std::ostream* out_stream):
    out_stream_(out_stream)
{
}

AstGraphVisitorNotImplemented::AstGraphVisitorNotImplemented(const std::string& visitor,
                                                             const std::string& node):
    std::invalid_argument("Visitor" + visitor + " not implemented for node type " + node)
{
}

void AstGraphVisitor::NewTreeGraph(const std::string& tree_name)
{
    *out_stream_ << "digraph " + tree_name + " {\n";
    *out_stream_ << "node[style = filled]\n";
}

void AstGraphVisitor::EndTreeGraph()
{
    *out_stream_ << "}\n";
}

void AstGraphVisitor::setOutStream(std::ostream* outStream)
{
    out_stream_ = outStream;
}
} // namespace Antares::Solver::Visitors
