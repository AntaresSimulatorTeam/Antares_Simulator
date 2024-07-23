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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "nodal-optimization.h"

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
NodalOptimization::NodalOptimization(const AnyString& areaname) : pOriginalAreaName(areaname)
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

                ctx.area->spreadUnsuppliedEnergyCost
                  = source->spreadUnsuppliedEnergyCost; // MBO 23/05/2014 - #23
                ctx.area->spreadSpilledEnergyCost
                  = source->spreadSpilledEnergyCost; // MBO 23/05/2014 - #23
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
