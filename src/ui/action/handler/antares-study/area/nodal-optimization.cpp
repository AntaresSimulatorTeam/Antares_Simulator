// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "nodal-optimization.h"

namespace Antares::Action::AntaresStudy::Area
{
NodalOptimization::NodalOptimization(const AnyString& areaname):
    pOriginalAreaName(areaname)
{
    pInfos.caption << "Nodal Optimization";
}

NodalOptimization::~NodalOptimization()
{
}

bool NodalOptimization::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The nodal optimization will be copied";
        break;
    default:
        pInfos.state = stDisabled;
    }

    return true;
}

bool NodalOptimization::performWL(Context& ctx)
{
    if (ctx.area && ctx.extStudy && ctx.area->ui)
    {
        Data::Area* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        // check the pointer + make sure that this is not the same memory area
        if (source)
        {
            if (source != ctx.area)
            {
                ctx.area->nodalOptimization = source->nodalOptimization;
                ctx.area->thermal.unsuppliedEnergyCost = source->thermal.unsuppliedEnergyCost;
                ctx.area->thermal.spilledEnergyCost = source->thermal.spilledEnergyCost;

                ctx.area->spreadUnsuppliedEnergyCost = source->spreadUnsuppliedEnergyCost; // MBO
                                                                                           // 23/05/2014
                                                                                           // - #23
                ctx.area->spreadSpilledEnergyCost = source
                                                      ->spreadSpilledEnergyCost; // MBO 23/05/2014 -
                                                                                 // #23
            }
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Area
