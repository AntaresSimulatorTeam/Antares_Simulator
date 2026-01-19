// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "portField.h"

namespace Antares::ModelerStudy::SystemModel
{

class PortType final
{
public:
    PortType(const std::string& id,
             std::vector<PortField>&& fields,
             const std::string& areaConnectionFieldId = ""):
        id_(id),
        fields_(std::move(fields))
    {
        if (!areaConnectionFieldId.empty())
        {
            if (!std::ranges::any_of(fields_,
                                     [areaConnectionFieldId](const auto& field)
                                     { return field.Id() == areaConnectionFieldId; }))
            {
                throw std::invalid_argument(
                  "Field \"" + areaConnectionFieldId
                  + "\" selected for area connections was not defined in PortType \"" + id_
                  + "\".");
            }
            areaConnectionFieldId_ = areaConnectionFieldId;
        }
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

    bool operator==(const PortType& other) const = default;

private:
    std::string id_;

    std::vector<PortField> fields_;

    std::optional<std::string> areaConnectionFieldId_;
};

} // namespace Antares::ModelerStudy::SystemModel
