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

#include <string>
#include <vector>

#include "portField.h"

namespace Antares::Solver::ObjectModel
{

class PortType
{
public:
    PortType(const std::string& id,
             const std::string& description,
             std::vector<PortField>&& fields):
        id_(id),
        description_(description),
        fields_(std::move(fields))
    {
    }

    const std::string& Id() const
    {
        return id_;
    }

    const std::string& Description() const
    {
        return description_;
    }

    const std::vector<PortField>& Fields() const
    {
        return fields_;
    }

private:
    std::string id_;
    std::string description_;

    std::vector<PortField> fields_;
};

} // namespace Antares::Solver::ObjectModel
