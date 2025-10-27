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

#include "antares/modeler/optimConfig/optimConfig.h"

#include <unordered_map>

#include <antares/exception/LoadingError.hpp>
#include <antares/exception/RuntimeError.hpp>

namespace Antares::Modeler::Config
{

std::ostream& operator<<(std::ostream& os, Location loc)
{
    switch (loc)
    {
    case Location::MASTER:
        return os << "MASTER";
    case Location::MASTER_AND_SUBPROBLEMS:
        return os << "MASTER_AND_SUBPROBLEMS";
    case Location::SUBPROBLEMS:
        return os << "SUBPROBLEMS";
    }
    throw Error::RuntimeError("Unknown Location enum value");
}

void OptimConfig::checkDuplicateModelIds() const
{
    std::unordered_map<std::string, int> modelIds;
    for (const auto& model: models_)
    {
        modelIds[model.id()]++;
    }

    for (const auto& [id, count]: modelIds)
    {
        if (count > 1)
        {
            throw Error::Duplicates("OptimConfig contains multiple models with ID \"" + id + "\".");
        }
    }
}

} // namespace Antares::Modeler::Config
