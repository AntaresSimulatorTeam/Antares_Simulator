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
#include <set>

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

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

void GetLegend(const std::map<std::string, std::map<const Nodes::Node*, unsigned int>>& nodeIds,
               std::ostream& os)
{
    os << R"raw(subgraph cluster_legend {
label = "Legend";
style = dashed;
fontsize = 16;
color = lightgrey;
node [shape=plaintext];

)raw";

    auto order = nodeIds.size() <=> 1;
    if (order == std::strong_ordering::greater)
    {
        for (auto it = nodeIds.begin(), next_it = std::next(it); next_it != nodeIds.end();
             ++it, ++next_it)
        {
            auto current_legend_id = std::string("legend_") + it->first;
            auto next_legend_id = std::string("legend_") + next_it->first;
            os << current_legend_id << " [ label =\" " << it->first << ": " << it->second.size()
               << "\"]\n";
            os << current_legend_id << " -> " << next_legend_id << " [style=invis];\n";
        }
    }
    else if (order == std::strong_ordering::equal)
    {
        auto first_element_key = nodeIds.begin()->first;
        os << "legend_" << first_element_key << " [ label =\" " << first_element_key << ": "
           << nodeIds.begin()->second.size() << "\"]\n";
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
    return "AstGraphVisitor";
}

unsigned int AstDOTStyleVisitor::getNodeID(const Nodes::Node* node)
{
    const auto& node_name = node->name();
    if (nodeIds_.find(node_name) == nodeIds_.end())
    {
        nodeIds_[node->name()][node] = ++nodeCount_;
    }
    else if (auto& id_map = nodeIds_[node_name]; id_map.find(node) == id_map.end())
    {
        id_map[node] = ++nodeCount_;
    }

    return nodeIds_[node->name()][node];
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
    // Graph title showing the total number of nodes
    os << "label=\"AST Diagram(Total nodes : " << nodeCount_ << ")\"\n";
    os << "labelloc = \"t\"\n";
    GetLegend(nodeIds_, os);
    os << "}\n";
    //  TODO
    //    nodeCount_ = 0;
    //    nodeIds_.clear();
}

std::ostream& operator<<(std::ostream& os,
                         const std::pair<AstDOTStyleVisitor&, Nodes::Node*>& visitorExpr)
{
    auto& [visitor, root] = visitorExpr;
    visitor.NewTreeGraph(os);
    visitor.dispatch(root, os);
    visitor.EndTreeGraph(os);
    return os;
}
} // namespace Antares::Solver::Visitors
