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
