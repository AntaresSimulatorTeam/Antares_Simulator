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
#include <optional>
#include <typeindex>
#include <vector>

#include <antares/logs/logs.h>
#include <antares/solver/expressions/nodes/Node.h>
#include <antares/solver/expressions/nodes/NodesForwardDeclaration.h>

namespace Antares::Solver::Nodes
{
namespace
{
template<class RetT, class VisitorT, class NodeT, class... Args>
std::optional<RetT> tryVisit(const Node& node, VisitorT& visitor, Args... args)
{
    auto* x = dynamic_cast<const NodeT*>(&node);
    return visitor.visit(*x, args...);
}
} // namespace

struct TheVisitorIsNotImplementedForYourNode: std::out_of_range
{
    // custom ctor with the visitor and node name will be defined in upcoming branch
    using std::out_of_range::out_of_range;
};

template<class R, class... Args>
class NodeVisitor
{
public:
    virtual ~NodeVisitor() = default;
    using FunctionT = std::optional<R> (*)(const Node&, NodeVisitor<R, Args...>&, Args... args);

    /**
     * Creates a map associating node types with corresponding visitor functions.
     *
     * @tparam NodeTypes A variadic pack of node types to be included in the map.
     * @return An `std::unordered_map` containing the associations between node types and their
     * corresponding visitor functions.
     */
    template<class... NodeTypes>
    std::unordered_map<std::type_index, FunctionT> NodesVisitList() const
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

    R dispatch(const Node& node, Args... args)
    {
        const static std::unordered_map<std::type_index, FunctionT>
          nodeDispatchFunctions = NodesVisitList<AddNode,
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
            const auto& node_type = typeid(node);
            if (auto ret = nodeDispatchFunctions.at(node_type)(node, *this, args...);
                ret.has_value())
            {
                return ret.value();
            }

            using namespace std::string_literals;
            // node type is unknown or not found in the map: Throw an exception
            throw TheVisitorIsNotImplementedForYourNode("Visitor ("s + typeid(*this).name()
                                                        + ") cannot handle nodes of type ("
                                                        + node_type.name() + ").");
        }
        catch (std::exception&)
        {
            logs.error() << "Antares::Solver::Visitor: could not visit the node!";
            throw;
        }
    }

    virtual R visit(const AddNode&, Args... args) = 0;
    virtual R visit(const SubtractionNode&, Args... args) = 0;
    virtual R visit(const MultiplicationNode&, Args... args) = 0;
    virtual R visit(const DivisionNode&, Args... args) = 0;
    virtual R visit(const EqualNode&, Args... args) = 0;
    virtual R visit(const LessThanOrEqualNode&, Args... args) = 0;
    virtual R visit(const GreaterThanOrEqualNode&, Args... args) = 0;
    virtual R visit(const NegationNode&, Args... args) = 0;
    virtual R visit(const LiteralNode&, Args... args) = 0;
    virtual R visit(const VariableNode&, Args... args) = 0;
    virtual R visit(const ParameterNode&, Args... args) = 0;
    virtual R visit(const PortFieldNode&, Args... args) = 0;
    virtual R visit(const ComponentVariableNode&, Args... args) = 0;
    virtual R visit(const ComponentParameterNode&, Args... args) = 0;
};
} // namespace Antares::Solver::Nodes
