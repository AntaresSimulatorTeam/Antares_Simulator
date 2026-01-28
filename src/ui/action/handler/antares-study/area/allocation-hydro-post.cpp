// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "allocation-hydro-post.h"
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Area
{
AllocationHydroPost::AllocationHydroPost(IAction* parent, const AnyString& areaname):
    pAction(parent),
    pOriginalAreaName(areaname)
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
                targetArea->hydro.allocation.copyFrom(source->hydro.allocation,
                                                      *ctx.extStudy,
                                                      ctx.areaNameMapping,
                                                      *ctx.study);
            }
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Area
