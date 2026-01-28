// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/ParentNode.h>

namespace Antares::Expressions::Nodes
{
class TimeSumNode final: public ParentNode
{
public:
    /**
     * @brief Constructs a Time Sum node with the specified 'from', 'to' and 'expression' operands.
     *
     * @param from The 'from' operand.
     * @param to The 'to' operand.
     * @param expression The 'expression' operand.
     */
    explicit TimeSumNode(Node* from, Node* to, Node* expression);
    std::string name() const override;

    /**
     * @brief Retrieves a pointer to the right operand.
     *
     * @return A pointer to the right operand.
     */
    Node* from() const;
    /**
     * @brief Retrieves a pointer to the right operand.
     *
     * @return A pointer to the right operand.
     */

    /**
     * @brief Retrieves a pointer to the ' operand.
     *
     * @return A pointer to the left operand.
     */
    Node* to() const;
    Node* expression() const;
};
} // namespace Antares::Expressions::Nodes
