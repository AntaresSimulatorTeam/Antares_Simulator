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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "correlation-post.h"

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
CorrelationPost::CorrelationPost(IAction* parent, Data::TimeSeriesType ts, const AnyString& areaname) :
 pAction(parent), pType(ts), pOriginalAreaName(areaname)
{
    pInfos.caption << "Correlation for " << areaname << " (deferred)";
}

CorrelationPost::~CorrelationPost()
{
}

bool CorrelationPost::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = (pAction->state() == stReady) ? stReady : stNothingToDo;
    return true;
}

bool CorrelationPost::performWL(Context& ctx)
{
    ctx.message.clear() << "Updating the correlation coefficients...";
    ctx.updateMessageUI(ctx.message);

    if (ctx.extStudy)
    {
        Data::Area* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        // check the pointer + make sure that this is not the same memory area
        if (source)
        {
            switch (pType)
            {
            case Data::timeSeriesLoad:
                ctx.study->preproLoadCorrelation.copyFrom(ctx.extStudy->preproLoadCorrelation,
                                                          *ctx.extStudy,
                                                          pOriginalAreaName,
                                                          ctx.areaNameMapping,
                                                          *ctx.study);
                break;
            case Data::timeSeriesSolar:
                ctx.study->preproSolarCorrelation.copyFrom(ctx.extStudy->preproSolarCorrelation,
                                                           *ctx.extStudy,
                                                           pOriginalAreaName,
                                                           ctx.areaNameMapping,
                                                           *ctx.study);
                break;
            case Data::timeSeriesWind:
                ctx.study->preproWindCorrelation.copyFrom(ctx.extStudy->preproWindCorrelation,
                                                          *ctx.extStudy,
                                                          pOriginalAreaName,
                                                          ctx.areaNameMapping,
                                                          *ctx.study);
                break;
            case Data::timeSeriesHydro:
                ctx.study->preproHydroCorrelation.copyFrom(ctx.extStudy->preproHydroCorrelation,
                                                           *ctx.extStudy,
                                                           pOriginalAreaName,
                                                           ctx.areaNameMapping,
                                                           *ctx.study);
                break;
            default:
                break;
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
