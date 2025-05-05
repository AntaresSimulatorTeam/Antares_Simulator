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

#include <algorithm>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "portField.h"

namespace Antares::ModelerStudy::SystemModel
{

class PortType
{
public:
    PortType(const std::string& id,
             std::vector<PortField>&& fields,
             const std::string& areaConnectionFieldId = ""):
        id_(id),
        fields_(std::move(fields))
    {
        if (areaConnectionFieldId.empty())
        {
            areaConnectionFieldId_ = std::nullopt;
        }
        else
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
            areaConnectionFieldId_ = std::optional(areaConnectionFieldId);
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
