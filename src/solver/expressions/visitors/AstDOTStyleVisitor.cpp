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
#include "antares/solver/expressions/visitors/AstDOTStyleVisitor.h"

#include <algorithm>

#include "antares/solver/expressions/nodes/ExpressionsNodes.h"

namespace Antares::Solver::Visitors
{
namespace NodeStyle
{
static constexpr BoxStyle SumStyle{"aqua", "hexagon", "filled, solid"};
static constexpr BoxStyle BinaryStyle{"aqua", "oval", "filled, rounded"};
static constexpr BoxStyle ComparisonStyle{"yellow", "diamond", "filled"};
static constexpr BoxStyle NegationStyle{"tomato", "invtriangle", "filled, solid"};
static constexpr BoxStyle LiteralStyle{"lightgray", "box", "filled, solid"};
static constexpr BoxStyle VariableStyle{"gold", "box", "filled, solid"};
static constexpr BoxStyle ParameterStyle{"wheat", "box", "filled, solid"};
static constexpr BoxStyle ComponentParameterStyle{"springgreen", "octagon", "filled, solid"};
static constexpr BoxStyle ComponentVariableStyle{"goldenrod", "octagon", "filled, solid"};
static constexpr BoxStyle PortFieldStyle{"olive", "component", "filled, solid"};
} // namespace NodeStyle

void makeLegendTitle(std::ostream& os)
{
    os << "subgraph cluster_legend {\n"
       << "label = \"Legend\";\n"
       << "style = dashed;\n"
       << "fontsize = 16;\n"
       << "color = lightgrey;\n"
       << "node [shape=plaintext];\n\n";
}

void ProcessElementLegend(const std::string& element_name, size_t size, std::ostream& os)
{
    os << "legend_" << element_name << " [ label =\" " << element_name << ": " << size << "\"]\n";
}

void AddFiliation(std::ostream& os, const std::string& parent_id, const std::string& child_id)
{
    os << "legend_" << parent_id << " -> " << "legend_" << child_id << " [style=invis];\n";
}

void AstDOTStyleVisitor::makeLegend(std::ostream& os)
{
    if (nbNodesPerType_.empty())
    {
        return;
    }

    ProcessElementLegend(nbNodesPerType_.begin()->first, nbNodesPerType_.begin()->second, os);
    for (auto it = std::next(nbNodesPerType_.begin()); it != nbNodesPerType_.end(); ++it)
    {
        auto prev_it = std::prev(it);
        AddFiliation(os, prev_it->first, it->first);
        ProcessElementLegend(it->first, it->second, os);
    }
    os << "}\n";
}

void AstDOTStyleVisitor::visit(const Nodes::SumNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id, "+", NodeStyle::SumStyle, os);
    for (auto* child: node->getOperands())
    {
        auto childId = getNodeID(child);
        os << "  " << id << " -> " << childId << ";\n";
        dispatch(child, os);
    }
}

void AstDOTStyleVisitor::visit(const Nodes::SubtractionNode* node, std::ostream& os)
{
    processBinaryOperation(node, "-", NodeStyle::BinaryStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::MultiplicationNode* node, std::ostream& os)
{
    processBinaryOperation(node, "*", NodeStyle::BinaryStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::DivisionNode* node, std::ostream& os)
{
    processBinaryOperation(node, "/", NodeStyle::BinaryStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::EqualNode* node, std::ostream& os)
{
    processBinaryOperation(node, "==", NodeStyle::ComparisonStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::LessThanOrEqualNode* node, std::ostream& os)
{
    processBinaryOperation(node, "<=", NodeStyle::ComparisonStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::GreaterThanOrEqualNode* node, std::ostream& os)
{
    processBinaryOperation(node, ">=", NodeStyle::ComparisonStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::VariableNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id, "Var(" + node->value() + ")", NodeStyle::VariableStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::ParameterNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id, "Param(" + node->value() + ")", NodeStyle::ParameterStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::LiteralNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id, std::to_string(node->value()), NodeStyle::LiteralStyle, os);
}

void AstDOTStyleVisitor::visit(const Nodes::NegationNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id, "-", NodeStyle::NegationStyle, os);
    auto childId = getNodeID(node->child());
    os << "  " << id << " -> " << childId << ";\n";
    dispatch(node->child(), os);
}

void AstDOTStyleVisitor::visit(const Nodes::PortFieldNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id,
             "PF(" + node->getPortName() + "," + node->getFieldName() + ")",
             NodeStyle::PortFieldStyle,
             os);
}

void AstDOTStyleVisitor::visit(const Nodes::PortFieldSumNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id,
             "PFSUM(" + node->getPortName() + "," + node->getFieldName() + ")",
             NodeStyle::PortFieldStyle,
             os);
}

void AstDOTStyleVisitor::visit(const Nodes::ComponentVariableNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id,
             "CV(" + node->getComponentId() + "," + node->getComponentName() + ")",
             NodeStyle::ComponentVariableStyle,
             os);
}

void AstDOTStyleVisitor::visit(const Nodes::ComponentParameterNode* node, std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id,
             "CP(" + node->getComponentId() + "," + node->getComponentName() + ")",
             NodeStyle::ComponentParameterStyle,
             os);
}

std::string AstDOTStyleVisitor::name() const
{
    return "AstDOTStyleVisitor";
}

unsigned int AstDOTStyleVisitor::getNodeID(const Nodes::Node* node)
{
    if (nodeIds_.find(node) == nodeIds_.end())
    {
        nodeIds_[node] = ++nodeCount_;
    }
    return nodeIds_[node];
}

void AstDOTStyleVisitor::computeNumberNodesPerType()
{
    for (const auto& [node, _]: nodeIds_)
    {
        ++nbNodesPerType_[node->name()];
    }
}

void AstDOTStyleVisitor::emitNode(unsigned int id,
                                  const std::string& label,
                                  const BoxStyle& box_style,
                                  std::ostream& os)
{
    os << "  " << id << " [label=\"" << label << "\", shape=\"" << box_style.shape << "\", style=\""
       << box_style.style << "\", color=\"" << box_style.color << "\"];\n";
}

// Process binary operation nodes like Add, Subtract, etc.
void AstDOTStyleVisitor::processBinaryOperation(const Nodes::BinaryNode* node,
                                                const std::string& label,
                                                const BoxStyle& box_style,
                                                std::ostream& os)
{
    auto id = getNodeID(node);
    emitNode(id, label, box_style, os);

    const Nodes::Node* left = node->left();
    const Nodes::Node* right = node->right();

    auto leftId = getNodeID(left);
    auto rightId = getNodeID(right);

    os << "  " << id << " -> " << leftId << ";\n";
    os << "  " << id << " -> " << rightId << ";\n";

    dispatch(left, os);
    dispatch(right, os);
}

void AstDOTStyleVisitor::NewTreeGraph(std::ostream& os, const std::string& tree_name)
{
    os << "digraph " + tree_name + " {\n";
    os << "node[style = filled]\n";
}

void AstDOTStyleVisitor::EndTreeGraph(std::ostream& os)
{
    computeNumberNodesPerType();

    // Graph title showing the total number of nodes
    os << "label=\"AST Diagram(Total nodes : " << nodeCount_ << ")\"\n";
    os << "labelloc = \"t\"\n";

    makeLegendTitle(os);
    makeLegend(os);
    os << "}\n";

    nodeCount_ = 0;
    nodeIds_.clear();
    nbNodesPerType_.clear();
}

void AstDOTStyleVisitor::operator()(std::ostream& os, const Nodes::Node* root)
{
    NewTreeGraph(os);
    dispatch(root, os);
    EndTreeGraph(os);
}
} // namespace Antares::Solver::Visitors
