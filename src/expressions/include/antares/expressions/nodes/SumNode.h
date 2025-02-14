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

#include "antares/expressions/nodes/Node.h"

namespace Antares::Expressions::Nodes
{

template<typename T>
concept NodePtr = std::same_as<T, Node*>;

template<typename T, typename... Args>
requires(std::convertible_to<Args, T> && ...)
std::vector<T> createVector(T first, Args... args)
{
    return std::vector<T>{first, args...};
}

class SumNode: public Node
{
public:
    template<typename... NodePtr>
    explicit SumNode(NodePtr... operands)
    {
        if constexpr (sizeof...(NodePtr))
        {
            operands_ = createVector(static_cast<Node*>(operands)...);
        }
    }

    /**
     * @brief Constructs a sum node with the specified operands.
     *
     * @param operands The operands, collected in a vector
     */
    explicit SumNode(const std::vector<Node*>& operands);

    /**
     * @brief Constructs a sum node with the specified operands. Vector is moved.
     *
     * @param operands The operands, collected in a vector
     */
    explicit SumNode(std::vector<Node*>&& operands):
        operands_(std::move(operands))
    {
    }

    /**
     * @brief Retrieves the operands of the sum.
     *
     * @return A vector of pointers to the operands of the sum.
     */
    const std::vector<Node*>& getOperands() const;

    Node* operator[](std::size_t idx) const;

    size_t size() const;

    std::string name() const override
    {
        return "SumNode";
    }

private:
    std::vector<Node*> operands_ = {};
};
} // namespace Antares::Expressions::Nodes
