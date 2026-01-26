// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <fmt/format.h>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "portField.h"

namespace Antares::ModelerStudy::SystemModel
{
inline std::optional<std::string> getOptionalConnectionField(const std::vector<PortField>& fields,
                                                             const std::string& portTypeId,
                                                             const std::string& connectionFieldId,
                                                             const std::string& nameOfTheConnection)
{
    if (!connectionFieldId.empty())
    {
        if (!std::ranges::any_of(fields,
                                 [connectionFieldId](const auto& field)
                                 { return field.Id() == connectionFieldId; }))
        {
            const auto msg = fmt::format(
              "Field '{}' selected for {} connections was not defined in PortType '{}'.",
              connectionFieldId,
              nameOfTheConnection,
              portTypeId);
            throw std::invalid_argument(msg);
        }
        return connectionFieldId;
    }
    return std::nullopt;
}

class PortType final
{
public:
    PortType(const std::string& id,
             std::vector<PortField>&& fields,
             const std::string& areaConnectionFieldId = "",
             const std::string& thermalCapacityConnectionField = ""):
        id_(id),
        fields_(std::move(fields))
    {
        areaConnectionFieldId_ = getOptionalConnectionField(fields_,
                                                            id,
                                                            areaConnectionFieldId,
                                                            "area");
        thermalCapacityConnectionFieldId_ = getOptionalConnectionField(
          fields_,
          id,
          thermalCapacityConnectionField,
          "thermal capacity");
    }

    const std::string& Id() const
    {
        return id_;
    }

    const std::vector<PortField>& Fields() const
    {
        return fields_;
    }

    const std::optional<std::string>& AreaConnectionFieldId() const
    {
        return areaConnectionFieldId_;
    }

    const std::optional<std::string>& ThermalCapacityConnectionFieldId() const
    {
        return thermalCapacityConnectionFieldId_;
    }

    bool operator==(const PortType& other) const = default;

private:
    std::string id_;

    std::vector<PortField> fields_;

    std::optional<std::string> areaConnectionFieldId_;
    std::optional<std::string> thermalCapacityConnectionFieldId_;
};

} // namespace Antares::ModelerStudy::SystemModel
