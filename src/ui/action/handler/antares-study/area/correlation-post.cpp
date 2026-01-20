// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "correlation-post.h"

namespace Antares::Action::AntaresStudy::Area
{
CorrelationPost::CorrelationPost(IAction* parent,
                                 Data::TimeSeriesType ts,
                                 const AnyString& areaname):
    pAction(parent),
    pType(ts),
    pOriginalAreaName(areaname)
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

} // namespace Antares::Action::AntaresStudy::Area
