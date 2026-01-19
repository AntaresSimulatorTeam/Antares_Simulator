// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__
#define __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__

#include <antares/logs/logs.h>

namespace Antares::TSGenerator
{

// forward declaration
// Hydro - see hydro.cpp
bool GenerateHydroTimeSeries(Data::Study& study, IResultWriter& writer);

template<>
inline bool GenerateTimeSeries<Data::timeSeriesHydro>(Data::Study& study, IResultWriter& writer)
{
    return GenerateHydroTimeSeries(study, writer);
}

// --- TS Generators using XCast ---
template<enum Data::TimeSeriesType T>
bool GenerateTimeSeries(Data::Study& study, IResultWriter& writer)
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
    xcast->year = 0;

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
    Destroy<T>(study);
    return r;
}

// TODO REMOVE
template<Data::TimeSeriesType T>
void Destroy(Data::Study& study)
{
    auto* xcast = reinterpret_cast<XCast::XCast*>(
      study.cacheTSGenerator[Data::TimeSeriesBitPatternIntoIndex<T>::value]);
    if (not xcast)
    {
        return;
    }

    logs.info() << "  Releasing the " << Data::TimeSeriesToCStr<T>::Value() << " TS Generator";
    study.cacheTSGenerator[Data::TimeSeriesBitPatternIntoIndex<T>::value] = nullptr;
    study.destroyTSGeneratorData<T>();
    delete xcast;
    xcast = nullptr;
}

} // namespace Antares::TSGenerator

#endif // __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__
