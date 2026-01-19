// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

class ParentNode: public Node
{
public:
    template<typename... NodePtr>
    explicit ParentNode(NodePtr... operands)
    {
        if constexpr (sizeof...(NodePtr))
        {
            operands_ = createVector(static_cast<Node*>(operands)...);
        }
    }

    /**
     * @brief Constructs a parent node with the specified operands.
     *
     * @param operands The operands, collected in a vector
     */
    explicit ParentNode(const std::vector<Node*>& operands);

    /**
     * @brief Constructs a parent node with the specified operands. Vector is moved.
     *
     * @param operands The operands, collected in a vector
     */
    explicit ParentNode(std::vector<Node*>&& operands):
        operands_(std::move(operands))
    {
    }

    /**
     * @brief Retrieves the operands of the parent node.
     *
     * @return A vector of pointers to the operands of the parent node.
     */
    const std::vector<Node*>& getOperands() const;
    const std::vector<const Node*> getConstOperands() const;

    Node* operator[](std::size_t idx) const;

    size_t size() const;

private:
    std::vector<Node*> operands_ = {};
};
} // namespace Antares::Expressions::Nodes
