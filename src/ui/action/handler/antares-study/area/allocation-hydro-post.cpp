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

#include "allocation-hydro-post.h"
#include <antares/utils/utils.h>

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
AllocationHydroPost::AllocationHydroPost(IAction* parent, const AnyString& areaname) :
 pAction(parent), pOriginalAreaName(areaname)
{
    pInfos.caption << "Hydro Allocation for " << areaname << " (deferred)";
}

AllocationHydroPost::~AllocationHydroPost()
{
}

bool AllocationHydroPost::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = (pAction->state() == stReady) ? stReady : stNothingToDo;
    return true;
}

bool AllocationHydroPost::performWL(Context& ctx)
{
    ctx.message.clear() << "Updating the hydro allocation coefficients...";
    ctx.updateMessageUI(ctx.message);

    if (ctx.extStudy)
    {
        auto* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        // check the pointer + make sure that this is not the same memory area
        if (source)
        {
            // we have to recompute the target area since there is no longer a valid
            // pointer to the target area (post-processing)
            Data::AreaName targetID;
            TransformNameIntoID(ctx.areaNameMapping[pOriginalAreaName], targetID);

            auto* targetArea = ctx.study->areas.find(targetID);
            if (targetArea && targetArea != source)
            {
                targetArea->hydro.allocation.copyFrom(
                  source->hydro.allocation, *ctx.extStudy, ctx.areaNameMapping, *ctx.study);
            }
            return true;
        }
    }
    return false;
}

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
