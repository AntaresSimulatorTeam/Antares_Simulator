// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "reserves.h"

namespace Antares::Action::AntaresStudy::Area
{
Reserves::Reserves(const AnyString& areaname):
    pOriginalAreaName(areaname)
{
    pInfos.caption << "Reserves";
}

Reserves::~Reserves()
{
}

bool Reserves::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The reserves will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
    }

    return true;
}

bool Reserves::performWL(Context& ctx)
{
    if (ctx.area && ctx.extStudy && ctx.area->ui)
    {
        Data::Area* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        // check the pointer + make sure that this is not the same memory area
        if (source)
        {
            if (source != ctx.area)
            {
                ctx.area->reserves = source->reserves;
                source->reserves.unloadFromMemory();
            }
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Area
