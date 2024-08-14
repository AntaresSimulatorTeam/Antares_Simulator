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
#include <vector>
#include <optional>

#include <antares/logs/logs.h>
#include <antares/solver/expressions/nodes/NodesForwardDeclaration.h>

namespace Antares::Solver::Nodes
{
namespace
{
template<class RetT, class VisitorT, class NodeT>
std::optional<RetT> tryType(const Node& node, VisitorT& visitor)
{
    if (auto x = dynamic_cast<const NodeT*>(&node))
    {
        return visitor.visit(*x);
    }
    else
    {
        return {};
    }
}
} // namespace

template<class R>
class NodeVisitor
{
public:
    virtual ~NodeVisitor() = default;

    R dispatch(const Node& node)
    {
        using FunctionT = std::optional<R> (*)(const Node&, NodeVisitor<R>&);
        std::vector<FunctionT> functions{&tryType<R, NodeVisitor<R>, AddNode>,
                                         &tryType<R, NodeVisitor<R>, SubtractionNode>,
                                         &tryType<R, NodeVisitor<R>, MultiplicationNode>,
                                         &tryType<R, NodeVisitor<R>, DivisionNode>,
                                         &tryType<R, NodeVisitor<R>, EqualNode>,
                                         &tryType<R, NodeVisitor<R>, LessThanOrEqualNode>,
                                         &tryType<R, NodeVisitor<R>, GreaterThanOrEqualNode>,
                                         &tryType<R, NodeVisitor<R>, NegationNode>,
                                         &tryType<R, NodeVisitor<R>, ParameterNode>,
                                         &tryType<R, NodeVisitor<R>, VariableNode>,
                                         &tryType<R, NodeVisitor<R>, LiteralNode>,
                                         &tryType<R, NodeVisitor<R>, PortFieldNode>,
                                         &tryType<R, NodeVisitor<R>, ComponentVariableNode>,
                                         &tryType<R, NodeVisitor<R>, ComponentParameterNode>};
        for (auto f: functions)
        {
            if (auto ret = f(node, *this); ret.has_value())
            {
                return ret.value();
            }
        }
        logs.error() << "Antares::Solver::Nodes Visitor: unsupported Node!";
        return R();
    }

    virtual R visit(const AddNode&) = 0;

    virtual R visit(const SubtractionNode&) = 0;
    virtual R visit(const MultiplicationNode&) = 0;
    virtual R visit(const DivisionNode&) = 0;
    virtual R visit(const EqualNode&) = 0;
    virtual R visit(const LessThanOrEqualNode&) = 0;
    virtual R visit(const GreaterThanOrEqualNode&) = 0;
    virtual R visit(const NegationNode&) = 0;
    virtual R visit(const LiteralNode&) = 0;
    virtual R visit(const VariableNode&) = 0;
    virtual R visit(const ParameterNode&) = 0;
    virtual R visit(const PortFieldNode&) = 0;
    virtual R visit(const ComponentVariableNode&) = 0;
    virtual R visit(const ComponentParameterNode&) = 0;
};

} // namespace Antares::Solver::Nodes
