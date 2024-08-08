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
#include <stdexcept>
#include <string>

#include <antares/solver/expressions/nodes/Node.h>

namespace Antares::Solver::Expressions
{
struct ParentNodeException: std::out_of_range
{
    using std::out_of_range::out_of_range;
};

template<std::size_t N>
class ParentNode: public Node
{
public:
    explicit ParentNode(const std::array<Node*, N>& children):
        children_(children)
    {
    }

    Node* operator[](std::size_t idx) const
    {
        if (children_.empty() || idx >= children_.size())
        {
            throw ParentNodeException(
              "Antares::Solver::Expressions::ParentNode can't get the child node at position"
              + std::to_string(idx));
        }
        else
        {
            return children_[idx];
        }
    }

private:
    std::array<Node*, N> children_;
};
} // namespace Antares::Solver::Expressions
