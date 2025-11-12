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

namespace Antares::Expressions::Visitors
{
template<class R, class Op>
R applyOperation(const R& a, const R& b, Op op);

template<class Visitor, class Op>
auto variadicFunction(Visitor& visitor, const Nodes::FunctionNode* node, Op op)
{
    const auto& operands = node->getOperands();
    // we know that min has at least two child
    auto result(visitor.dispatch(operands.at(0)));
    for (int i = 1; i < operands.size(); ++i)
    {
        const auto* operand = operands.at(i);
        result = applyOperation(result, visitor.dispatch(operand), op);
    }
    return result;
}
} // namespace Antares::Expressions::Visitors
