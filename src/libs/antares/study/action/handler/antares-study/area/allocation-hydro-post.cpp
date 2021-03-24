/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "allocation-hydro-post.h"

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

            auto* targetArea = ctx.study.areas.find(targetID);
            if (targetArea && targetArea != source)
            {
                targetArea->hydro.allocation.copyFrom(
                  source->hydro.allocation, *ctx.extStudy, ctx.areaNameMapping, ctx.study);
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
