// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <sstream>

#include "port.h"

namespace Antares::ModelerStudy::SystemModel
{

// Define an enumeration for the role of a ConnectionEnd
enum class FieldRole
{
    Sender,
    Receiver
};

inline std::ostream& operator<<(std::ostream& os, const SystemModel::FieldRole& role)
{
    return role == SystemModel::FieldRole::Sender ? os << "Sender" : os << "Receiver";
}

using PortFieldsRole = std::map<PortField, FieldRole>;

class Component;

class ConnectionEnd final
{
public:
    ConnectionEnd(Component* component, const Port* port);
    [[nodiscard]] const Component* component() const;
    [[nodiscard]] const Port* port() const;

private:
    const Component* component_;
    const Port* port_;
};

} // namespace Antares::ModelerStudy::SystemModel
