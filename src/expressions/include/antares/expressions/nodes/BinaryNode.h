// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/ParentNode.h>

namespace Antares::Expressions::Nodes
{
class BinaryNode: public ParentNode
{
public:
    /**
     * @brief Constructs a binary node with the specified left and right operands.
     *
     * @param left The left operand.
     * @param right The right operand.
     * @note BinaryNode(n1, n2) and BinaryNode(n2, n1) are not equivalent.
     */
    explicit BinaryNode(Node* left, Node* right);

    /**
     * @brief Retrieves a pointer to the left operand.
     *
     * @return A pointer to the left operand.
     */
    Node* left() const;

    /**
     * @brief Retrieves a pointer to the right operand.
     *
     * @return A pointer to the right operand.
     */
    Node* right() const;
};
} // namespace Antares::Expressions::Nodes
