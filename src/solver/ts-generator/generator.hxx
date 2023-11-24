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
#ifndef __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__
#define __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__

#include <antares/logs/logs.h>

namespace Antares::Solver::TSGenerator
{
 
inline void ResizeGeneratedTimeSeries(Data::AreaList& areas, Data::Parameters& params)
{
    for (auto i = areas.begin(); i != areas.end(); ++i)
    {
        auto& area = *(i->second);

        // Load
        if (params.timeSeriesToRefresh & Data::timeSeriesLoad)
        {
            area.load.series.timeSeries.reset(params.nbTimeSeriesLoad, HOURS_PER_YEAR);
        }

        // Wind
        if (params.timeSeriesToRefresh & Data::timeSeriesWind)
        {
            area.wind.series.timeSeries.reset(params.nbTimeSeriesWind, HOURS_PER_YEAR);
        }

        // Solar
        if (params.timeSeriesToRefresh & Data::timeSeriesSolar)
        {
            area.solar.series.timeSeries.reset(params.nbTimeSeriesSolar, HOURS_PER_YEAR);
        }

        // Hydro
        if (params.timeSeriesToRefresh & Data::timeSeriesHydro)
        {
            area.hydro.series->resizeRORandSTORAGE(params.nbTimeSeriesHydro);
        }

        // Thermal
        auto end = area.thermal.list.mapping.end();
        for (auto it = area.thermal.list.mapping.begin(); it != end; ++it)
        {
            auto& cluster = *(it->second);
            bool globalThermalTSgeneration = params.timeSeriesToRefresh & Data::timeSeriesThermal;
            if (cluster.doWeGenerateTS(globalThermalTSgeneration))
                cluster.series.timeSeries.reset(params.nbTimeSeriesThermal, HOURS_PER_YEAR);
        }
    }
}


// forward declaration
// Hydro - see hydro.cpp
bool GenerateHydroTimeSeries(Data::Study& study, uint year, IResultWriter& writer);

template<>
inline bool GenerateTimeSeries<Data::timeSeriesHydro>(Data::Study& study,
                                                      uint year,
                                                      IResultWriter& writer)
{
    return GenerateHydroTimeSeries(study, year, writer);
}

// --- TS Generators using XCast ---
template<enum Data::TimeSeriesType T>
bool GenerateTimeSeries(Data::Study& study, uint year, IResultWriter& writer)
{
    auto* xcast = reinterpret_cast<XCast::XCast*>(
      study.cacheTSGenerator[Data::TimeSeriesBitPatternIntoIndex<T>::value]);

    if (not xcast)
    {
        logs.debug() << "Preparing the " << Data::TimeSeriesToCStr<T>::Value() << " TS Generator";
        xcast = new XCast::XCast(study, T, writer);
        study.cacheTSGenerator[Data::TimeSeriesBitPatternIntoIndex<T>::value] = xcast;
    }

    // The current year
    xcast->year = year;

    switch (T)
    {
    case Data::timeSeriesLoad:
        xcast->random = &(study.runtime->random[Data::seedTsGenLoad]);
        break;
    case Data::timeSeriesSolar:
        xcast->random = &(study.runtime->random[Data::seedTsGenSolar]);
        break;
    case Data::timeSeriesWind:
        xcast->random = &(study.runtime->random[Data::seedTsGenWind]);
        break;
    case Data::timeSeriesHydro:
        xcast->random = &(study.runtime->random[Data::seedTsGenHydro]);
        break;
    default:
        xcast->random = nullptr;
        assert(false and "invalid ts type");
    }

    // Run the generation of the time-series
    bool r = xcast->run();
    // Destroy if required the TS Generator
    Destroy<T>(study, year);
    return r;
}

template<enum Data::TimeSeriesType T>
void Destroy(Data::Study& study, uint year)
{
    auto* xcast = reinterpret_cast<XCast::XCast*>(
      study.cacheTSGenerator[Data::TimeSeriesBitPatternIntoIndex<T>::value]);
    if (not xcast)
        return;

    // releasing
    auto& parameters = study.parameters;

    bool shouldDestroy;
    switch (T)
    {
    case Data::timeSeriesLoad:
    {
        shouldDestroy = (parameters.refreshIntervalLoad > parameters.nbYears)
                        || year + parameters.refreshIntervalLoad > parameters.nbYears;
        break;
    }
    case Data::timeSeriesSolar:
    {
        shouldDestroy = (parameters.refreshIntervalSolar > parameters.nbYears)
                        || year + parameters.refreshIntervalSolar > parameters.nbYears;
        break;
    }
    case Data::timeSeriesHydro:
    {
        shouldDestroy = (parameters.refreshIntervalHydro > parameters.nbYears)
                        || year + parameters.refreshIntervalHydro > parameters.nbYears;
        break;
    }
    case Data::timeSeriesWind:
    {
        shouldDestroy = (parameters.refreshIntervalWind > parameters.nbYears)
                        || year + parameters.refreshIntervalWind > parameters.nbYears;
        break;
    }
    case Data::timeSeriesThermal:
    {
        shouldDestroy = (parameters.refreshIntervalThermal > parameters.nbYears)
                        || year + parameters.refreshIntervalThermal > parameters.nbYears;
        break;
    }
    default:
        shouldDestroy = true;
    }

    if (shouldDestroy)
    {
        logs.info() << "  Releasing the " << Data::TimeSeriesToCStr<T>::Value() << " TS Generator";
        study.cacheTSGenerator[Data::TimeSeriesBitPatternIntoIndex<T>::value] = nullptr;
        study.destroyTSGeneratorData<T>();
        delete xcast;
        xcast = nullptr;
    }
}

inline void DestroyAll(Data::Study& study)
{
    Solver::TSGenerator::Destroy<Data::timeSeriesLoad>(study, (uint)-1);
    Solver::TSGenerator::Destroy<Data::timeSeriesSolar>(study, (uint)-1);
    Solver::TSGenerator::Destroy<Data::timeSeriesWind>(study, (uint)-1);
    Solver::TSGenerator::Destroy<Data::timeSeriesHydro>(study, (uint)-1);
    Solver::TSGenerator::Destroy<Data::timeSeriesThermal>(study, (uint)-1);
}

} // namespace Antares::Solver::TSGenerator

#endif // __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__
