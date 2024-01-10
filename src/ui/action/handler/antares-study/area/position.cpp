/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "position.h"

using namespace Yuni;

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
Position::Position(const AnyString& areaname) : pOriginalAreaName(areaname)
{
    pInfos.caption << "Position";
}

Position::~Position()
{
}

bool Position::prepareWL(Context& ctx)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
    {
        const String& decalX = ctx.property["area.coordinate.x.offset"];
        const String& decalY = ctx.property["area.coordinate.y.offset"];
        if (!decalX && !decalY)
            pInfos.message << "The coordinates will be copied";
        else
            pInfos.message << "The coordinates will be copied (x:+" << decalX << ", y:+" << decalY
                           << ')';
        break;
    }
    default:
        pInfos.state = stDisabled;
        break;
    }

    return true;
}

bool Position::performWL(Context& ctx)
{
    if (ctx.area && ctx.extStudy && ctx.area->ui)
    {
        Data::Area* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        if (source && source->ui)
        {
            const String& decalX = ctx.property["area.coordinate.x.offset"];
            const String& decalY = ctx.property["area.coordinate.y.offset"];
            int dX = 0;
            int dY = 0;
            if (not decalX.empty())
            {
                if (!decalX.to(dX))
                    dX = 0;
            }
            if (not decalY.empty())
            {
                if (!decalY.to(dY))
                    dY = 0;
            }

            ctx.area->ui->x = source->ui->x + dX;
            ctx.area->ui->y = source->ui->y + dY;

            Data::Area* nearest
              = ctx.extStudy->areas.findFromPosition(ctx.area->ui->x, ctx.area->ui->y);
            while (nearest && nearest != ctx.area)
            {
                ctx.area->ui->x += 25;
                ctx.area->ui->y += -20;

                nearest = ctx.extStudy->areas.findFromPosition(ctx.area->ui->x, ctx.area->ui->y);
            }

            if (ctx.layerID != 0)
                ctx.area->ui->mapLayersVisibilityList.push_back(ctx.layerID);

            return true;
        }
    }
    return false;
}

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
