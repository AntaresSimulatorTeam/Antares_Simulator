// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "allocation-hydro.h"
#include "allocation-hydro-post.h"

namespace Antares::Action::AntaresStudy::Area
{
AllocationHydro::AllocationHydro(const AnyString& areaname):
    pOriginalAreaName(areaname)
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

} // namespace Antares::Action::AntaresStudy::Area
