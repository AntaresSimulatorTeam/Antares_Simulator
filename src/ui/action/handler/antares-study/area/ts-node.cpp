// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "ts-node.h"
#include <antares/study/fwd.h>

using namespace Yuni;

namespace Antares::Action::AntaresStudy::Area
{
NodeTimeseries::NodeTimeseries(Data::TimeSeriesType ts):
    pType(ts)
{
    // behavior
    pInfos.behavior = bhOverwrite;

    switch (ts)
    {
    case Data::timeSeriesLoad:
        pInfos.caption << "Load";
        break;
    case Data::timeSeriesSolar:
        pInfos.caption << "Solar";
        break;
    case Data::timeSeriesWind:
        pInfos.caption << "Wind";
        break;
    case Data::timeSeriesHydro:
        pInfos.caption << "Hydro";
        break;
    case Data::timeSeriesThermal:
        pInfos.caption << "Thermal";
        break;
    default:
        break;
    }
}

NodeTimeseries::~NodeTimeseries()
{
}

bool NodeTimeseries::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message = "The timeseries and the local data will be reset to default";
        break;
    default:
        pInfos.state = stNothingToDo;
    }
    return true;
}

bool NodeTimeseries::performWL(Context& ctx)
{
    if (!ctx.area)
    {
        return false;
    }
    switch (pType)
    {
    case Data::timeSeriesLoad:
        ctx.area->load.resetToDefault();
        break;
    case Data::timeSeriesSolar:
        ctx.area->solar.resetToDefault();
        break;
    case Data::timeSeriesWind:
        ctx.area->wind.resetToDefault();
        break;
    case Data::timeSeriesHydro:
        ctx.area->hydro.intraDailyModulation = 2.;
        ctx.area->hydro.interDailyBreakdown = 0.;
        assert(ctx.area->hydro.series);
        ctx.area->hydro.series->reset();
        assert(ctx.area->hydro.prepro);
        ctx.area->hydro.prepro->reset();
        break;
    case Data::timeSeriesThermal:
        if (ctx.cluster)
        {
            ctx.cluster->series.timeSeries.reset(1, HOURS_PER_YEAR);
            ctx.cluster->prepro->reset();
            ctx.cluster->ecoInput.reset();
        }
        break;
    default:
        break;
    }
    return true;
}

} // namespace Antares::Action::AntaresStudy::Area
