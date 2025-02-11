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

#include <antares/expressions/nodes/Node.h>

namespace Antares::Expressions::Nodes
{
class BinaryNode: public Node
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

private:
    /**
     * @brief A pointer to the left operand.
     */
    Node* leftOperand_ = nullptr;

    /**
     * @brief A pointer to the right operand.
     */
    Node* rightOperand_ = nullptr;
};
} // namespace Antares::Expressions::Nodes
