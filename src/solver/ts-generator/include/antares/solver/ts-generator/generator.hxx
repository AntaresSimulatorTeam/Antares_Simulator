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
#ifndef __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__
#define __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__

#include <antares/logs/logs.h>

namespace Antares::TSGenerator
{

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
        xcast->random = &(study.runtime.random[Data::seedTsGenLoad]);
        break;
    case Data::timeSeriesSolar:
        xcast->random = &(study.runtime.random[Data::seedTsGenSolar]);
        break;
    case Data::timeSeriesWind:
        xcast->random = &(study.runtime.random[Data::seedTsGenWind]);
        break;
    case Data::timeSeriesHydro:
        xcast->random = &(study.runtime.random[Data::seedTsGenHydro]);
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
    {
        return;
    }

    // releasing
    auto& parameters = study.parameters;

    bool shouldDestroy;
    switch (T)
    {
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

} // namespace Antares::TSGenerator

#endif // __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__
