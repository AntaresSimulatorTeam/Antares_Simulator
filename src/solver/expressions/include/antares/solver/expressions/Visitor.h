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
#include <antares/solver/expressions/NodesForwardDeclaration.h>

namespace Antares::Solver::Expressions
{

template<class R>
class Visitor
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
    virtual ~Visitor() = default;

    R dispatch(const Node& node)
    {
        using Function = std::optional<R> (Antares::Solver::Expressions::Visitor<R>::*)(
          const Antares::Solver::Expressions::Node&);
        static const std::array<Function, 7> tryFunctions{
          &Visitor<R>::tryType<AddNode>,
          &Visitor<R>::tryType<NegationNode>,
          &Visitor<R>::tryType<ParameterNode>,
          &Visitor<R>::tryType<LiteralNode>,
          &Visitor<R>::tryType<PortFieldNode>,
          &Visitor<R>::tryType<ComponentVariableNode>,
          &Visitor<R>::tryType<ComponentParameterNode>};
        for (auto f: tryFunctions)
        {
            if (auto x = (this->*f)(node))
            {
                return std::move(*x);
            }
        }
        logs.error() << "Antares::Solver::Expressions Visitor: unsupported Node!";
        return R();
    }

private:
    virtual R visit(const AddNode&) = 0;
    virtual R visit(const NegationNode&) = 0;
    virtual R visit(const LiteralNode&) = 0;
    virtual R visit(const ParameterNode&) = 0;
    virtual R visit(const PortFieldNode&) = 0;
    virtual R visit(const ComponentVariableNode&) = 0;
    virtual R visit(const ComponentParameterNode&) = 0;
};

} // namespace Antares::Solver::Expressions
