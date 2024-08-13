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
#include <array>
#include <optional>
#include <string>

#include <antares/logs/logs.h>
#include <antares/solver/expressions/nodes/NodesForwardDeclaration.h>

namespace Antares::Solver::Nodes
{

template<class R>
class NodeVisitor
{
private:
    template<class T>
    std::optional<R> tryType(const Node& node)
    {
        if (const T* x = dynamic_cast<const T*>(&node))
        {
            return visit(*x); // on appelle la version 'T' (virtuelle pure)
        }
        return {};
    }

public:
    virtual ~NodeVisitor() = default;

    R dispatch(const Node& node)
    {
        using Function = std::optional<R> (Antares::Solver::Nodes::NodeVisitor<R>::*)(
          const Antares::Solver::Nodes::Node&);
        static const std::array<Function, 16> tryFunctions{
          &NodeVisitor<R>::tryType<AddNode>,
          &NodeVisitor<R>::tryType<SubtractionNode>,
          &NodeVisitor<R>::tryType<MultiplicationNode>,
          &NodeVisitor<R>::tryType<DivisionNode>,
          &NodeVisitor<R>::tryType<EqualNode>,
          &NodeVisitor<R>::tryType<LessThanOrEqualNode>,
          &NodeVisitor<R>::tryType<GreaterThanOrEqualNode>,
          &NodeVisitor<R>::tryType<NegationNode>,
          &NodeVisitor<R>::tryType<ParameterNode>,
          &NodeVisitor<R>::tryType<VariableNode>,
          &NodeVisitor<R>::tryType<LiteralNode>,
          &NodeVisitor<R>::tryType<PortFieldNode>,
          &NodeVisitor<R>::tryType<ComponentVariableNode>,
          &NodeVisitor<R>::tryType<ComponentParameterNode>};
        for (auto f: tryFunctions)
        {
            if (auto x = (this->*f)(node))
            {
                return x.value();
            }
        }
        logs.error() << "Antares::Solver::Nodes Visitor: unsupported Node!";
        return R();
    }

private:
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
