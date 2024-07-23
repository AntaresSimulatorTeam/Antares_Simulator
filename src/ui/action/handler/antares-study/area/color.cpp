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

#include "color.h"

using namespace Yuni;

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
Color::Color(const AnyString& areaname) : pOriginalAreaName(areaname)
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
                ctx.area->ui->color[i] = source->ui->color[i];
            // even the cache
            ctx.area->ui->cacheColorHSV = source->ui->cacheColorHSV;
            return true;
        }
    }
    return false;
}

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
