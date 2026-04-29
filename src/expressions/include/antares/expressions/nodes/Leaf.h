// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/Node.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a leaf node in a syntax tree.
 *
 * @tparam T The type of the value stored in the leaf node.
 */
template<class T>
class Leaf: public Node
{
public:
    /**
     * @brief Constructs a leaf node with the specified value.
     *
     * @param value The value to store in the leaf node.
     */
    explicit Leaf(const T& value):
        value_(value)
    {
    }

    /**
     * @brief Retrieves the value stored in the leaf node.
     *
     * @return The value stored in the leaf node.
     */
    T value() const
    {
        return value_;
    }

private:
    /**
     * @brief The value stored in the leaf node.
     */
    T value_;
};

} // namespace Antares::Expressions::Nodes
