/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
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
**
** SPDX-License-Identifier: MPL-2.0
*/
#ifndef __ANTARES_LIBS_STUDY_STORE_TIMESERIES_HXX__
#define __ANTARES_LIBS_STUDY_STORE_TIMESERIES_HXX__

namespace Antares::Data
{
template<int TimeSeriesT>
void singleAreaStoreTimeseriesNumbers(Solver::IResultWriter& writer, const Area& area)
{
    switch (TimeSeriesT)
    {
    // Load
    case timeSeriesLoad:
        storeTimeseriesNumbersForLoad(writer, area);
        break;
        // Solar
    case timeSeriesSolar:
        storeTimeseriesNumbersForSolar(writer, area);
        break;
        // Hydro
    case timeSeriesHydro:
        storeTimeseriesNumbersForHydro(writer, area);
        break;
        // Wind
    case timeSeriesWind:
        storeTimeseriesNumbersForWind(writer, area);
        break;
        // Thermal
    case timeSeriesThermal:
        storeTimeseriesNumbersForThermal(writer, area);
        break;
        // Renewable
    case timeSeriesRenewable:
        storeTimeseriesNumbersForRenewable(writer, area);
        break;
        // Transmission capacities (NTC)
    case timeSeriesTransmissionCapacities:
        storeTimeseriesNumbersForTransmissionCapacities(writer, area);
        break;
    case timeSeriesCount:
    default:
        break;
    }
}

template<int TimeSeriesT>
void storeTimeseriesNumbers(Solver::IResultWriter& writer, const AreaList& area)
{
    // Each area in the list
    area.each([&writer](const Area& a) { singleAreaStoreTimeseriesNumbers<TimeSeriesT>(writer, a); });
}
} //namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_STORE_TIMESERIES_HXX__
