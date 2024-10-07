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

#include "antares/solver/simulation/solver.data.h"

#include <antares/study/parameters.h>

namespace Antares::Solver::Private::Simulation
{
namespace
{
template<int TS>
inline static bool CheckTSToRefresh(const Antares::Data::Parameters& g)
{
    return (TS & g.timeSeriesToRefresh) && (TS & g.timeSeriesToGenerate);
}

} // namespace

void CacheData::initialize(const Antares::Data::Parameters& parameters)
{
    haveToRefreshTSSolar = CheckTSToRefresh<Data::timeSeriesSolar>(parameters)
                           && parameters.renewableGeneration.isAggregated();
    haveToRefreshTSHydro = CheckTSToRefresh<Data::timeSeriesHydro>(parameters);
    haveToRefreshTSWind = CheckTSToRefresh<Data::timeSeriesWind>(parameters)
                          && parameters.renewableGeneration.isAggregated();
    haveToRefreshTSThermal = CheckTSToRefresh<Data::timeSeriesThermal>(parameters);

    refreshIntervalSolar = parameters.refreshIntervalSolar;
    refreshIntervalHydro = parameters.refreshIntervalHydro;
    refreshIntervalWind = parameters.refreshIntervalWind;
    refreshIntervalThermal = parameters.refreshIntervalThermal;
}

} // namespace Antares::Solver::Private::Simulation
