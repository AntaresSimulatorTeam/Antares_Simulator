// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include "portType.h"

namespace Antares::ModelerStudy::SystemModel
{

enum class FieldRole
{
    Sender,
    Receiver
};

inline std::ostream& operator<<(std::ostream& os, const SystemModel::FieldRole& role)
{
    return role == SystemModel::FieldRole::Sender ? os << "Sender" : os << "Receiver";
}

class Port final
{
public:
    Port(const std::string& id, const PortType& type):
        id_(id),
        type_(type)
    {
        for (const auto& field: type.Fields())
        {
            field_roles_[field.Id()] = FieldRole::Receiver;
        }
    }

    const std::string& Id() const
    {
        return id_;
    }

    const PortType& Type() const
    {
        return type_;
    }

    void setFieldRole(const std::string& field_id, FieldRole role)
    {
        field_roles_[field_id] = role;
    }

    FieldRole fieldRole(const std::string& field_id) const
    {
        return field_roles_.at(field_id);
    }

private:
    std::string id_;
    PortType type_;
    std::map<std::string, FieldRole> field_roles_;
};

} // namespace Antares::ModelerStudy::SystemModel
