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

#include <antares/expressions/nodes/FunctionNode.h>
#include <antares/expressions/visitors/NodeVisitor.h>

namespace Antares::Expressions::Visitors
{

template<typename T>
concept HasSizeMethod = requires(const T& t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};

template<HasSizeMethod T>
std::size_t getMaxSize(const std::vector<T>& elements)
{
    std::size_t maxSize = 0;
    for (const auto& element: elements)
    {
        maxSize = std::max(maxSize, element.size());
    }
    return maxSize;
}

template<class R>
std::vector<R> variadicFunction(NodeVisitor<R>& visitor, const Nodes::FunctionNode* node)
{
    const auto& operands = node->getOperands();
    std::vector<R> result;
    result.reserve(operands.size());
    for (const auto* operand: operands)
    {
        result.push_back(visitor.dispatch(operand));
    }
    return result;
}
} // namespace Antares::Expressions::Visitors
