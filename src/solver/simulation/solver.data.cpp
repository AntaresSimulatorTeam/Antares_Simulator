/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
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
    haveToRefreshTSLoad = CheckTSToRefresh<Data::timeSeriesLoad>(parameters);
    haveToRefreshTSSolar = CheckTSToRefresh<Data::timeSeriesSolar>(parameters)
                           && parameters.renewableGeneration.isAggregated();
    haveToRefreshTSHydro = CheckTSToRefresh<Data::timeSeriesHydro>(parameters);
    haveToRefreshTSWind = CheckTSToRefresh<Data::timeSeriesWind>(parameters)
                          && parameters.renewableGeneration.isAggregated();
    haveToRefreshTSThermal = CheckTSToRefresh<Data::timeSeriesThermal>(parameters);

    refreshIntervalLoad = parameters.refreshIntervalLoad;
    refreshIntervalSolar = parameters.refreshIntervalSolar;
    refreshIntervalHydro = parameters.refreshIntervalHydro;
    refreshIntervalWind = parameters.refreshIntervalWind;
    refreshIntervalThermal = parameters.refreshIntervalThermal;
}

} // namespace Antares::Solver::Private::Simulation
