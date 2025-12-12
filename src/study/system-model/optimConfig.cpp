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

#include "antares/study/system-model/optimConfig.h"

namespace Antares::Modeler::Config
{

const char* LocationToStr(Location loc)
{
    switch (loc)
    {
    case Location::MASTER:
        return "master";
    case Location::MASTER_AND_SUBPROBLEMS:
        return "master-and-subproblems";
    case Location::SUBPROBLEMS:
        return "subproblems";
    default:
        return "Unknown";
    }
}

bool AreLocationsCompatibleForFillers(Location lhs, Location rhs)
{
    switch (rhs)
    {
    case Location::MASTER:
        return lhs == Location::MASTER || lhs == Location::MASTER_AND_SUBPROBLEMS;
    case Location::SUBPROBLEMS:
        return lhs == Location::SUBPROBLEMS || lhs == Location::MASTER_AND_SUBPROBLEMS;
    case Location::MASTER_AND_SUBPROBLEMS:
        return true;
    default:
        return false;
    }
}

// stricter for MASTER_AND_SUBPROLEMS
bool AreLocationsCompatibleForExpressions(Location lhs, Location rhs)
{
    switch (rhs)
    {
    case Location::MASTER:
        return lhs == Location::MASTER || lhs == Location::MASTER_AND_SUBPROBLEMS;
    case Location::SUBPROBLEMS:
        return lhs == Location::SUBPROBLEMS || lhs == Location::MASTER_AND_SUBPROBLEMS;
    case Location::MASTER_AND_SUBPROBLEMS:
        return lhs == Location::MASTER_AND_SUBPROBLEMS;
    default:
        return false;
    }
}

} // namespace Antares::Modeler::Config
