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
#include <typeindex>
#include <vector>

#include <antares/logs/logs.h>
#include <antares/solver/expressions/nodes/NodesForwardDeclaration.h>

namespace Antares::Solver::Nodes
{
namespace
{
template<class RetT, class VisitorT, class NodeT, class... Args>
RetT tryVisit(const Node& node, VisitorT& visitor, Args... args)
{
    auto* x = dynamic_cast<const NodeT*>(&node);
    return visitor.visit(*x, args...);
}
} // namespace
template<class R, class... Args>
class NodeVisitor;

template<class R, class... Args>
struct NodeDispatchFunctionsProvider
{
    using FunctionT = R (*)(const Node&, NodeVisitor<R, Args...>&, Args... args);

    /**
     * Creates a map associating node types with corresponding visitor functions.
     *
     * @tparam NodeTypes A variadic pack of node types to be included in the map.
     * @return An `std::unordered_map` containing the associations between node types and their
     * corresponding visitor functions.
     */
    template<class... NodeTypes>
    static auto NodesVisitList()
    {
        std::unordered_map<std::type_index, FunctionT> nodeDispatchFunctions;
        (
          [&nodeDispatchFunctions] {
              nodeDispatchFunctions[typeid(NodeTypes)] = &tryVisit<R,
                                                                   NodeVisitor<R, Args...>,
                                                                   NodeTypes>;
          }(),
          ...);
        return nodeDispatchFunctions;
    }
};

template<class R, class... Args>
class NodeVisitor
{
public:
    virtual ~NodeVisitor() = default;

    /**
     * Dispatches a node to an appropriate visitor function based on its type.
     *
     * This method uses a map that associates node types
     * with their corresponding visitor functions. It attempts to find the visitor function
     * for the provided `node`. If a match is found, the corresponding
     * visitor function is called with the node, and any
     * additional arguments (`args...`).
     *
     * @param node A reference to the Node object to be visited.
     * @param args Variadic template arguments to be passed to the visitor functions.
     * @return The return value of the visitor function.
     *
     */
    R dispatch(const Node& node, Args... args)
    {
        const static auto nodeDispatchFunctions = NodeDispatchFunctionsProvider<R, Args...>::
          template NodesVisitList<AddNode,
                                  SubtractionNode,
                                  MultiplicationNode,
                                  DivisionNode,
                                  EqualNode,
                                  LessThanOrEqualNode,
                                  GreaterThanOrEqualNode,
                                  NegationNode,
                                  ParameterNode,
                                  VariableNode,
                                  LiteralNode,
                                  PortFieldNode,
                                  ComponentVariableNode,
                                  ComponentParameterNode>();
        try
        {
            return nodeDispatchFunctions.at(typeid(node))(node, *this, args...);
        }
        catch (std::exception&)
        {
            logs.error() << "Antares::Solver::Visitor: could not visit the node!";
            throw;
        }
    }

    /**
     * @brief Visits an AddNode and processes its children.
     *
     * @param node A reference to the AddNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the AddNode.
     */
    virtual R visit(const AddNode&, Args... args) = 0;

    /**
     * @brief Visits a SubtractionNode and processes its children.
     *
     * @param node A reference to the SubtractionNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the SubtractionNode.
     */
    virtual R visit(const SubtractionNode&, Args... args) = 0;

    /**
     * @brief Visits a MultiplicationNode and processes its children.
     *
     * @param node A reference to the MultiplicationNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the MultiplicationNode.
     */
    virtual R visit(const MultiplicationNode&, Args... args) = 0;

    /**
     * @brief Visits a DivisionNode and processes its children.
     *
     * @param node A reference to the DivisionNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the DivisionNode.
     */
    virtual R visit(const DivisionNode&, Args... args) = 0;

    /**
     * @brief Visits an EqualNode and processes its children.
     *
     * @param node A reference to the EqualNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the EqualNode.
     */
    virtual R visit(const EqualNode&, Args... args) = 0;

    /**
     * @brief Visits a LessThanOrEqualNode and processes its children.
     *
     * @param node A reference to the LessThanOrEqualNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the LessThanOrEqualNode.
     */
    virtual R visit(const LessThanOrEqualNode&, Args... args) = 0;

    /**
     * @brief Visits a GreaterThanOrEqualNode and processes its children.
     *
     * @param node A reference to the GreaterThanOrEqualNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the GreaterThanOrEqualNode.
     */
    virtual R visit(const GreaterThanOrEqualNode&, Args... args) = 0;

    /**
     * @brief Visits a NegationNode and processes its child.
     *
     * @param node A reference to the NegationNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the NegationNode.
     */
    virtual R visit(const NegationNode&, Args... args) = 0;

    /**
     * @brief Visits a LiteralNode.
     *
     * @param node A reference to the LiteralNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the LiteralNode.
     */
    virtual R visit(const LiteralNode&, Args... args) = 0;

    /**
     * @brief Visits a VariableNode.
     *
     * @param node A reference to the VariableNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the VariableNode.
     */
    virtual R visit(const VariableNode&, Args... args) = 0;

    /**
     * @brief Visits a ParameterNode.
     *
     * @param node A reference to the ParameterNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the ParameterNode.
     */
    virtual R visit(const ParameterNode&, Args... args) = 0;

    /**
     * @brief Visits a PortFieldNode.
     *
     * @param node A reference to the PortFieldNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the PortFieldNode.
     */
    virtual R visit(const PortFieldNode&, Args... args) = 0;

    /**
     * @brief Visits a ComponentVariableNode.
     *
     * @param node A reference to the ComponentVariableNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the ComponentVariableNode.
     */
    virtual R visit(const ComponentVariableNode&, Args... args) = 0;

    /**
     * @brief Visits a ComponentParameterNode.
     *
     * @param node A reference to the ComponentParameterNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the ComponentParameterNode.
     */
    virtual R visit(const ComponentParameterNode&, Args... args) = 0;
};
} // namespace Antares::Solver::Nodes
