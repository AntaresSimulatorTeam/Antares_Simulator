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
