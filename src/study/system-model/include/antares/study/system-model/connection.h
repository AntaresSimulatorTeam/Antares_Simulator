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
#include "component.h"
#include "port.h"

namespace Antares::ModelerStudy::SystemModel
{

// Define an enumeration for the role of a ConnectionEntry
enum class FieldRole
{
    Sender,
    Receiver
};

/**
 *   @brief Overloads the << operator to print FieldRole as a string.
 *
 * @param os The output stream.
 * @param role The FieldRole value to print.
 * @return The modified output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const FieldRole& role)
{
    return role == FieldRole::Sender ? os << "Sender" : os << "Receiver";
}

using PortFieldsRole = std::map<PortField, FieldRole>;

/**
 * @class ConnectionEntry
 * @brief Represents an entry in a connection, associating a component with a port.
 *
 * The ConnectionEntry class is used to store a connection between a component and a port.
 * It ensures that both the component and port are valid (non-null) upon construction.
 */
class ConnectionEntry
{
public:
    /**
     * @brief Constructs a ConnectionEntry with the specified component and port.
     *
     * @param component A pointer to the component. Must not be null.
     * @param port A pointer to the port. Must not be null.
     * @param portFieldsRole
     * @throw std::invalid_argument if either component or port is null.
     */
    ConnectionEntry(const Component* component,
                    const Port* port,
                    const PortFieldsRole& portFieldsRole);

    /**
     * @brief Returns the portFieldsRole associated with this connection entry.
     *
     * @return A pointer to the portFieldsRole.
     */
    [[nodiscard]] const PortFieldsRole& portFieldsRole() const;

    /**
     * @brief Returns the component associated with this connection entry.
     *
     * @return A pointer to the component.
     */
    [[nodiscard]] const Component* component() const;

    [[nodiscard]] const Port* port() const;

private:
    const Component* component_; ///< Pointer to the associated component.
    const Port* port_;           ///< Pointer to the associated port.
    PortFieldsRole portFieldsRole_;
};

} // namespace Antares::ModelerStudy::SystemModel
