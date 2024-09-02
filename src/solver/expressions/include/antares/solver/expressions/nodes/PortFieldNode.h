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

#include <memory>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
/**
 * @brief Represents a port field node in a syntax tree.
 */
class PortFieldNode: public Node
{
public:
    /**
     * @brief Constructs a port field node with the specified port and field names.
     *
     * @param port_name The port name.
     * @param field_name The field name.
     */
    explicit PortFieldNode(const std::string& port_name, const std::string& field_name);

    /**
     * @brief Retrieves the port name.
     *
     * @return The port name.
     */
    const std::string& getPortName() const;

    /**
     * @brief Retrieves the field name.
     *
     * @return The field name.
     */
    const std::string& getFieldName() const;

    bool operator==(const PortFieldNode& other) const = default;

private:
    std::string port_name_;
    std::string field_name_;
};
} // namespace Antares::Solver::Nodes
