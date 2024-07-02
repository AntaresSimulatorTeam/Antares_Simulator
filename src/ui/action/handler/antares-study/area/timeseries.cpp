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

#include "timeseries.h"

using namespace Yuni;

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
DataTimeseries::DataTimeseries(Data::TimeSeriesType ts, const AnyString& areaname) :
 pType(ts), pOriginalAreaName(areaname)
{
    switch (ts)
    {
    case Data::timeSeriesLoad:
        pInfos.caption << "Load : Timeseries";
        break;
    case Data::timeSeriesSolar:
        pInfos.caption << "Solar : Timeseries";
        break;
    case Data::timeSeriesWind:
        pInfos.caption << "Wind : Timeseries";
        break;
    case Data::timeSeriesHydro:
        pInfos.caption << "Hydro : Timeseries";
        break;
    case Data::timeSeriesThermal:
        pInfos.caption << "Thermal : Timeseries";
        break;
    default:
        break;
    }
}

DataTimeseries::DataTimeseries(Data::TimeSeriesType ts,
                               const AnyString& areaname,
                               const AnyString& clustername) :
 pType(ts), pOriginalAreaName(areaname), pOriginalPlantName(clustername)
{
    // With this additional parameter, it can only be related to the thermal data
    assert(ts == Data::timeSeriesThermal);

    pInfos.caption << "Thermal : Timeseries";
}

DataTimeseries::~DataTimeseries()
{
}

void DataTimeseries::datagridCaption(String& title)
{
    ThreadingPolicy::MutexLocker locker(*this);
    if (pParent)
        title = pParent->caption();
}

void DataTimeseries::registerViewsWL(Context& ctx)
{
    switch (pType)
    {
    case Data::timeSeriesLoad:
        ctx.view["2:Load"]["1:TS"] = this;
        break;
    case Data::timeSeriesSolar:
        ctx.view["3:Solar"]["1:TS"] = this;
        break;
    case Data::timeSeriesWind:
        ctx.view["4:Wind"]["1:TS"] = this;
        break;
    case Data::timeSeriesHydro:
        ctx.view["5:Hydro"]["1:TS"] = this;
        break;
    case Data::timeSeriesThermal:
        ctx.view["6:Thermal"]["1:TS"] = this;
        break;
    default:
        break;
    }
}

bool DataTimeseries::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The time-series will be copied from '" << pOriginalAreaName << '"';
        break;
    default:
        pInfos.state = stNothingToDo;
    }
    return true;
}

bool DataTimeseries::performWL(Context& ctx)
{
    if (ctx.area && ctx.extStudy && ctx.area->ui)
    {
        Data::Area* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        // check the pointer + make sure that this is not the same memory area
        if (source)
        {
            if (source != ctx.area)
            {
                switch (pType)
                {
                case Data::timeSeriesLoad:
                {
                    ctx.area->load.series.timeSeries = source->load.series.timeSeries;
                    source->load.series.timeSeries.unloadFromMemory();
                    break;
                }
                case Data::timeSeriesSolar:
                {
                    ctx.area->solar.series.timeSeries = source->solar.series.timeSeries;
                    source->solar.series.timeSeries.unloadFromMemory();
                    break;
                }
                case Data::timeSeriesWind:
                {
                    ctx.area->wind.series.timeSeries = source->wind.series.timeSeries;
                    source->wind.series.timeSeries.unloadFromMemory();
                    break;
                }
                case Data::timeSeriesHydro:
                {
                    ctx.area->hydro.series->copyGenerationTS(*source->hydro.series);
                    break;
                }
                case Data::timeSeriesThermal:
                {
                    if (ctx.cluster && ctx.originalPlant && ctx.cluster != ctx.originalPlant)
                    {
                        ctx.cluster->series.timeSeries = ctx.originalPlant->series.timeSeries;
                        ctx.originalPlant->series.timeSeries.unloadFromMemory();
                    }
                    break;
                }
                default:
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

void DataTimeseries::behaviorToText(Behavior behavior, String& out)
{
    switch (behavior)
    {
    case bhOverwrite:
        out = "overwrite";
        break;
    case bhMerge:
        out = "merge";
        break;
    case bhSkip:
        out = "skip";
        break;
    case bhMax:
        out.clear();
        break;
    }
}

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
