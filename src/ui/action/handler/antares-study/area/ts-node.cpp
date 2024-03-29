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

#include "ts-node.h"
#include <antares/study/fwd.h>

using namespace Yuni;

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
NodeTimeseries::NodeTimeseries(Data::TimeSeriesType ts) : pType(ts)
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
        return false;
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

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
