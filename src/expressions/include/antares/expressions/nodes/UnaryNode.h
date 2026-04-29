// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/ParentNode.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a unary node in a syntax tree.
 */
class UnaryNode: public ParentNode
{
public:
    /**
     * @brief Constructs a unary node with the specified child.
     *
     * @param child The child node.
     */
    explicit UnaryNode(Node* child);
    /**
     * @brief Retrieves a pointer to the child node.
     *
     * @return A pointer to the child node.
     */
    Node* child() const;
};
} // namespace Antares::Expressions::Nodes
