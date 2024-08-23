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
#include <antares/solver/expressions/IName.h>
#include <antares/solver/expressions/nodes/NodesForwardDeclaration.h>

namespace Antares::Solver::Visitors
{

template<class RetT, class VisitorT, class NodeT, class... Args>
std::optional<RetT> tryVisit(const Nodes::Node& node, VisitorT& visitor, Args... args)
{
    if (auto* x = dynamic_cast<const NodeT*>(&node))
    {
        return visitor.visit(*x, args...);
    }
    return std::nullopt;
}

struct InvalidNode: std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

struct NotImplemented: std::invalid_argument
{
    NotImplemented(const IName& visitor, const IName& node):
        std::invalid_argument("Visitor" + visitor.name() + " not implemented for node type "
                              + node.name())
    {
    }
};

template<class R, class... Args>
class NodeVisitor: public IName
{
public:
    virtual ~NodeVisitor() = default;

    R dispatch(const Nodes::Node& node, Args... args)
    {
        using FunctionT = std::optional<R> (*)(const Nodes::Node&,
                                               NodeVisitor<R, Args...>&,
                                               Args... args);
        static const std::vector<FunctionT> allNodeVisitList{
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::AddNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::SubtractionNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::MultiplicationNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::DivisionNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::EqualNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::LessThanOrEqualNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::GreaterThanOrEqualNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::NegationNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::ParameterNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::VariableNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::LiteralNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::PortFieldNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::ComponentVariableNode>,
          &tryVisit<R, NodeVisitor<R, Args...>, Nodes::ComponentParameterNode>};
        for (auto f: allNodeVisitList)
        {
            if (auto ret = f(node, *this, args...); ret.has_value())
            {
                return ret.value();
            }
        }
        throw InvalidNode("Antares::Solver::Nodes Visitor: invalid Node!");
        return R();
    }

    virtual R visit(const Nodes::AddNode&, Args... args) = 0;
    virtual R visit(const Nodes::SubtractionNode&, Args... args) = 0;
    virtual R visit(const Nodes::MultiplicationNode&, Args... args) = 0;
    virtual R visit(const Nodes::DivisionNode&, Args... args) = 0;
    virtual R visit(const Nodes::EqualNode&, Args... args) = 0;
    virtual R visit(const Nodes::LessThanOrEqualNode&, Args... args) = 0;
    virtual R visit(const Nodes::GreaterThanOrEqualNode&, Args... args) = 0;
    virtual R visit(const Nodes::NegationNode&, Args... args) = 0;
    virtual R visit(const Nodes::LiteralNode&, Args... args) = 0;
    virtual R visit(const Nodes::VariableNode&, Args... args) = 0;
    virtual R visit(const Nodes::ParameterNode&, Args... args) = 0;
    virtual R visit(const Nodes::PortFieldNode&, Args... args) = 0;
    virtual R visit(const Nodes::ComponentVariableNode&, Args... args) = 0;
    virtual R visit(const Nodes::ComponentParameterNode&, Args... args) = 0;
};

} // namespace Antares::Solver::Visitors
