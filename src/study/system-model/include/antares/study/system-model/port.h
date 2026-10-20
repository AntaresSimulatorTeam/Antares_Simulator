// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <fmt/format.h>
#include <map>
#include <string>

#include "portType.h"

namespace Antares::ModelerStudy::SystemModel
{

enum class FieldRole
{
    Sender,
    Receiver
};

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

template<>
struct fmt::formatter<Antares::ModelerStudy::SystemModel::FieldRole>
    : fmt::formatter<std::string_view>
{
    auto format(Antares::ModelerStudy::SystemModel::FieldRole role, fmt::format_context& ctx) const
    {
        std::string_view name = role == Antares::ModelerStudy::SystemModel::FieldRole::Sender
                                  ? "Sender"
                                  : "Receiver";

        return fmt::formatter<std::string_view>::format(name, ctx);
    }
};
