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

#include "allocation-hydro.h"
#include "allocation-hydro-post.h"

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
AllocationHydro::AllocationHydro(const AnyString& areaname) : pOriginalAreaName(areaname)
{
    pInfos.caption = "Hydro Allocation";
}

AllocationHydro::~AllocationHydro()
{
}

bool AllocationHydro::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The hydro allocation coefficients will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool AllocationHydro::performWL(Context&)
{
    // Nothing to do
    return true;
}

void AllocationHydro::createPostActionsWL(const IAction::Ptr& node)
{
    // all actions on hydro allocation coefficients must be delayed
    // (we must have all areas, thus it must be done at the end of the merge)
    *node += new AllocationHydroPost(this, pOriginalAreaName);
}

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
