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

#include <map>
#include <ostream>
#include <utility>

#include "antares/expressions/visitors/NodeVisitor.h"

namespace Antares::Expressions::Visitors
{
/**
 * @struct BoxStyle
 * @brief Represents the style attributes for a box in a graph.
 *
 * This structure defines the visual properties of a box, such as its color,
 * shape, and style, which are used to customize the appearance of nodes in a graph.
 */
struct BoxStyle
{
    /**
     * @brief The color of the box.
     *
     * This attribute specifies the color of the box. It is a string view that
     * should contain a valid color name or code.
     */
    std::string_view color;

    /**
     * @brief The shape of the box.
     *
     * This attribute specifies the shape of the box. It is a string view that
     * should contain a valid shape description (e.g., "circle", "rectangle").
     */
    std::string_view shape;

    /**
     * @brief The style of the box.
     *
     * This attribute specifies the style of the box. It is a string view that
     * should contain a valid style description (e.g., "filled", "dotted").
     */
    std::string_view style;
};

/**
 * @class AstDOTStyleVisitor
 * @brief A visitor class for generating DOT style output for ASTs (Abstract Syntax Trees).
 *
 * This class extends the `NodeVisitor` template class to produce a DOT representation
 * of a given AST. The DOT format is commonly used for graph visualization tools,
 * such as Graphviz. The visitor generates the graph structure, including nodes and
 * edges, and outputs it to a stream.
 *
 * @tparam NodeVisitor A base class template for visiting nodes.
 * @tparam std::ostream& The type of the output stream where the DOT representation is written.
 * */
class AstDOTStyleVisitor: public NodeVisitor<void, std::ostream&>
{
public:
    /**
     * @brief Default constructor.
     */
    AstDOTStyleVisitor() = default;

    /**
     * @brief Begins a new tree graph.
     *
     * Initializes the DOT graph representation and sets the tree name.
     *
     * @param os The output stream to which the DOT representation is written.
     * @param tree_name The name of the tree graph. Defaults to "ExpressionTree".
     */
    void NewTreeGraph(std::ostream& os, const std::string& tree_name = "ExpressionTree");

    /**
     * @brief Ends the current tree graph.
     *
     * Finalizes the DOT graph representation.
     *
     * @param os The output stream to which the DOT representation is written.
     */
    void EndTreeGraph(std::ostream& os);

    /**
     * @brief Returns the name of this visitor.
     *
     * @return A string representing the name of the visitor.
     */
    std::string name() const override;

    /**
     * @brief Outputs the DOT representation of a node to a stream.
     *
     * This operator overload facilitates the use of the `AstDOTStyleVisitor` with a node
     * for direct streaming of the DOT representation.
     *
     * @param os The output stream to which the DOT representation is written.
     * @param root The root of the expression to be output.
     */
    void operator()(std::ostream& os, const Nodes::Node* root);

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
    void visit(const Nodes::PortFieldSumNode* node, std::ostream& os) override;
    void visit(const Nodes::ComponentVariableNode* node, std::ostream& os) override;
    void visit(const Nodes::ComponentParameterNode* node, std::ostream& os) override;

    void computeNumberNodesPerType();
    void makeLegend(std::ostream& os);

    /**
     * @brief Retrieves a unique ID for a given node.
     *
     * Generates or retrieves a unique identifier for the specified node.
     *
     * @param node The node for which to get the ID.
     * @return An integer representing the unique ID of the node.
     */
    unsigned int getNodeID(const Nodes::Node* node);

    /**
     * @brief Emits a node to the output stream.
     *
     * Writes the DOT representation of a node to the output stream with its associated label
     * and style.
     *
     * @param id The unique ID of the node.
     * @param label The label to be used for the node.
     * @param box_style The style to be applied to the node's box.
     * @param os The output stream to which the node representation is written.
     */
    void emitNode(unsigned int id,
                  const std::string& label,
                  const BoxStyle& box_style,
                  std::ostream& os);

    /**
     * @brief Processes a binary operation node.
     *
     * Handles the specific case of binary operation nodes by emitting the appropriate
     * DOT representation.
     *
     * @param node The binary operation node to be processed.
     * @param label The label to be used for the node.
     * @param box_style The style to be applied to the node's box.
     * @param os The output stream to which the node representation is written.
     */
    void processBinaryOperation(const Nodes::BinaryNode* node,
                                const std::string& label,
                                const BoxStyle& box_style,
                                std::ostream& os);

    /**
     * @brief A map of nodes to their unique IDs.
     *
     * This map is used to keep track of assigned IDs for each node in the AST.
     */
    std::map<const Nodes::Node*, unsigned int> nodeIds_;

    /**
     * @brief A map associating a number of instances to a type name.
     *
     * This map is used to keep track of assigned IDs for each node in the AST.
     */
    std::map<std::string, unsigned int> nbNodesPerType_;

    /**
     * @brief Counter for generating unique node IDs.
     *
     * This counter is incremented each time a new node ID is needed.
     */
    unsigned int nodeCount_ = 0;
};
} // namespace Antares::Expressions::Visitors
