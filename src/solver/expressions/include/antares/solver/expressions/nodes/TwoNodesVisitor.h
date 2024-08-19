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
#include <vector>

#include <antares/logs/logs.h>
#include <antares/solver/expressions/nodes/NodesForwardDeclaration.h>

namespace Antares::Solver::Nodes
{
namespace
{
template<class RetT, class VisitorT, class NodeT>
std::optional<RetT> tryType(const Node& node1, const Node& node2, VisitorT& visitor)
{
    auto x = dynamic_cast<const NodeT*>(&node1);
    auto y = dynamic_cast<const NodeT*>(&node2);
    if (x && y && (typeid(*x) == typeid(*y)))
    {
        return visitor.visit(*x, *y);
    }
    else
    {
        return {};
    }
}
} // namespace

template<class R>
class TwoNodesVisitor
{
public:
    virtual ~TwoNodesVisitor() = default;

    R dispatch(const Node& node1, const Node& node2)
    {
        using FunctionT = std::optional<R> (*)(const Node&, const Node&, TwoNodesVisitor<R>&);
        std::vector<FunctionT> functions{&tryType<R, TwoNodesVisitor<R>, AddNode>,
                                         &tryType<R, TwoNodesVisitor<R>, SubtractionNode>,
                                         &tryType<R, TwoNodesVisitor<R>, MultiplicationNode>,
                                         &tryType<R, TwoNodesVisitor<R>, DivisionNode>,
                                         &tryType<R, TwoNodesVisitor<R>, EqualNode>,
                                         &tryType<R, TwoNodesVisitor<R>, LessThanOrEqualNode>,
                                         &tryType<R, TwoNodesVisitor<R>, GreaterThanOrEqualNode>,
                                         &tryType<R, TwoNodesVisitor<R>, NegationNode>,
                                         &tryType<R, TwoNodesVisitor<R>, ParameterNode>,
                                         &tryType<R, TwoNodesVisitor<R>, VariableNode>,
                                         &tryType<R, TwoNodesVisitor<R>, LiteralNode>,
                                         &tryType<R, TwoNodesVisitor<R>, PortFieldNode>,
                                         &tryType<R, TwoNodesVisitor<R>, ComponentVariableNode>,
                                         &tryType<R, TwoNodesVisitor<R>, ComponentParameterNode>};
        for (auto f: functions)
        {
            if (auto ret = f(node1, node2, *this); ret.has_value())
            {
                return ret.value();
            }
        }
        logs.warning()
          << "Antares::Solver::Nodes TwoNodesVisitor: the inputs nodes must be of the same type!";
        return R{};
    }

    virtual R visit(const AddNode&, const AddNode&) = 0;
    virtual R visit(const SubtractionNode&, const SubtractionNode&) = 0;
    virtual R visit(const MultiplicationNode&, const MultiplicationNode&) = 0;
    virtual R visit(const DivisionNode&, const DivisionNode&) = 0;
    virtual R visit(const EqualNode&, const EqualNode&) = 0;
    virtual R visit(const LessThanOrEqualNode&, const LessThanOrEqualNode&) = 0;
    virtual R visit(const GreaterThanOrEqualNode&, const GreaterThanOrEqualNode&) = 0;
    virtual R visit(const NegationNode&, const NegationNode&) = 0;
    virtual R visit(const LiteralNode&, const LiteralNode&) = 0;
    virtual R visit(const VariableNode&, const VariableNode&) = 0;
    virtual R visit(const ParameterNode&, const ParameterNode&) = 0;
    virtual R visit(const PortFieldNode&, const PortFieldNode&) = 0;
    virtual R visit(const ComponentVariableNode&, const ComponentVariableNode&) = 0;
    virtual R visit(const ComponentParameterNode&, const ComponentParameterNode&) = 0;
};

} // namespace Antares::Solver::Nodes
