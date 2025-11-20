/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <string>

#include "antares/expressions/nodes/ParentNode.h"

namespace Antares::Expressions::Nodes
{
enum class FunctionNodeType
{
    reduced_cost,
    dual,
    max,
    min,
    pow,
};

std::string FunctionNodeTypeToString(FunctionNodeType type);

class FunctionNode final: public ParentNode
{
public:
    template<typename... NodePtr>
    explicit FunctionNode(FunctionNodeType type, NodePtr... operands):
        ParentNode(operands...),
        type_(type)

    {
    }

    explicit FunctionNode(FunctionNodeType type, const std::vector<Node*>& operands):
        ParentNode(operands),
        type_(type)
    {
    }

    explicit FunctionNode(FunctionNodeType type, std::vector<Node*>&& operands):
        ParentNode(std::move(operands)),
        type_(type)
    {
    }

    std::string name() const override
    {
        return "FunctionNode";
    }

    FunctionNodeType type() const
    {
        return type_;
    }

    std::string typeToString() const
    {
        return FunctionNodeTypeToString(type_);
    }

private:
    FunctionNodeType type_;
};
} // namespace Antares::Expressions::Nodes
