// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "color.h"

using namespace Yuni;

namespace Antares::Action::AntaresStudy::Area
{
Color::Color(const AnyString& areaname):
    pOriginalAreaName(areaname)
{
    pInfos.caption << "Color of the area";
    pInfos.behavior = bhSkip;
}

Color::~Color()
{
}

bool Color::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The color of the area will be copied";
        break;
    default:
        pInfos.state = stDisabled;
        break;
    }

    return true;
}

bool Color::performWL(Context& ctx)
{
    if (ctx.area && ctx.extStudy && ctx.area->ui)
    {
        auto* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        if (source && source->ui)
        {
            // color
            for (uint i = 0; i != 3; ++i)
            {
                ctx.area->ui->color[i] = source->ui->color[i];
            }
            // even the cache
            ctx.area->ui->cacheColorHSV = source->ui->cacheColorHSV;
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Area
