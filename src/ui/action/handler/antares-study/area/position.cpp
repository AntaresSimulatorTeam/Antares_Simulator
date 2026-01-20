// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "position.h"

using namespace Yuni;

namespace Antares::Action::AntaresStudy::Area
{
Position::Position(const AnyString& areaname):
    pOriginalAreaName(areaname)
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
        {
            pInfos.message << "The coordinates will be copied";
        }
        else
        {
            pInfos.message << "The coordinates will be copied (x:+" << decalX << ", y:+" << decalY
                           << ')';
        }
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
                {
                    dX = 0;
                }
            }
            if (not decalY.empty())
            {
                if (!decalY.to(dY))
                {
                    dY = 0;
                }
            }

            ctx.area->ui->x = source->ui->x + dX;
            ctx.area->ui->y = source->ui->y + dY;

            Data::Area* nearest = ctx.extStudy->areas.findFromPosition(ctx.area->ui->x,
                                                                       ctx.area->ui->y);
            while (nearest && nearest != ctx.area)
            {
                ctx.area->ui->x += 25;
                ctx.area->ui->y += -20;

                nearest = ctx.extStudy->areas.findFromPosition(ctx.area->ui->x, ctx.area->ui->y);
            }

            if (ctx.layerID != 0)
            {
                ctx.area->ui->mapLayersVisibilityList.push_back(ctx.layerID);
            }

            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Area
