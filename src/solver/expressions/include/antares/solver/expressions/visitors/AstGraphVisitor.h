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
#pragma once

#include <ostream>
#include <utility>

#include "antares/solver/expressions/visitors/NodeVisitor.h"

namespace Antares::Solver::Visitors
{

struct BoxStyle
{
    std::string label;
    std::string color = "azure";
    std::string shape = "box";
    std::string style = "rounded";
};

class AstGraphVisitor: public NodeVisitor<void, std::ostream&>
{
public:
    /**
     * @brief Default constructor, creates an evaluation visitor with no context.
     */
    AstGraphVisitor() = default;

    void NewTreeGraph(std::ostream& os, const std::string& tree_name = "ExpressionTree");
    void EndTreeGraph(std::ostream& os);

    std::string name() const override;

private:
    void visit(const Nodes::SumNode* node, std::ostream& os) override;
    void visit(const Nodes::SubtractionNode* node, std::ostream& os) override;
    void visit(const Nodes::MultiplicationNode* node, std::ostream& os) override;
    void visit(const Nodes::DivisionNode* node, std::ostream& os) override;
    void visit(const Nodes::EqualNode* node, std::ostream& os) override;
    void visit(const Nodes::LessThanOrEqualNode* node, std::ostream& os) override;
    void visit(const Nodes::GreaterThanOrEqualNode* node, std::ostream& os) override;
    void visit(const Nodes::NegationNode* node, std::ostream& os) override;
    void visit(const Nodes::VariableNode* node, std::ostream& os) override;
    void visit(const Nodes::ParameterNode* node, std::ostream& os) override;
    void visit(const Nodes::LiteralNode* node, std::ostream& os) override;
    void visit(const Nodes::PortFieldNode* node, std::ostream& os) override;
    void visit(const Nodes::ComponentVariableNode* node, std::ostream& os) override;
    void visit(const Nodes::ComponentParameterNode* node, std::ostream& os) override;

    int getNodeID(const Nodes::Node* node);
    void emitNode(int id, const BoxStyle& box_style, std::ostream& os);

    void processBinaryOperation(const Nodes::BinaryNode* node,
                                const BoxStyle& box_style,
                                std::ostream& os);

private:
    std::unordered_map<const Nodes::Node*, int> nodeIds_; // Mapping to store unique IDs for nodes
    int nodeCount_ = 0;                                   // Counter to assign unique node IDs
};

std::ostream& operator<<(std::ostream& os, std::pair<AstGraphVisitor&, Nodes::Node*>& visitorExpr);

} // namespace Antares::Solver::Visitors
