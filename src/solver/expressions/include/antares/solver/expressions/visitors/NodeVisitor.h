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
#include <stdexcept>
#include <vector>

#include <antares/logs/logs.h>
#include <antares/solver/expressions/nodes/NodesForwardDeclaration.h>

namespace Antares::Solver::Nodes
{

template<class RetT, class VisitorT, class NodeT, class... Args>
std::optional<RetT> tryVisit(const Node& node, VisitorT& visitor, Args... args)
{
    if (auto* x = dynamic_cast<const NodeT*>(&node))
    {
        return visitor.visit(*x, args...);
    }
    return std::nullopt;
}

struct NodeVistorException: std::runtime_error
{
    using std::runtime_error::runtime_error;
};

template<class R, class... Args>
class NodeVisitor
{
public:
    virtual ~NodeVisitor() = default;

    R dispatch(const Node& node, Args... args)
    {
        using FunctionT = std::optional<R> (*)(const Node&, NodeVisitor<R, Args...>&, Args... args);
        static const std::vector<FunctionT> allNodeVisitList{
          &tryVisit<R, NodeVisitor<R, Args...>, AddNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, SubtractionNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, MultiplicationNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, DivisionNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, EqualNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, LessThanOrEqualNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, GreaterThanOrEqualNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, NegationNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, ParameterNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, VariableNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, LiteralNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, PortFieldNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, ComponentVariableNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, ComponentParameterNode>};
        for (auto f: allNodeVisitList)
        {
            if (auto ret = f(node, *this, args...); ret.has_value())
            {
                return ret.value();
            }
        }
        throw NodeVistorException("Antares::Solver::Nodes Visitor: invalid Node!");
        return R();
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
