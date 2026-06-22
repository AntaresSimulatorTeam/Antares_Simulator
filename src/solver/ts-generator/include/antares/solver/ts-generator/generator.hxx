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
    logs.debug() << "Preparing the " << Data::TimeSeriesToCStr<T>::Value() << " TS Generator";
    auto xcast = std::make_unique<XCast::XCast>(study, T, writer);

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

    // TODO REMOVE
    study.destroyTSGeneratorData<T>();
    // Run the generation of the time-series
    return xcast->run();
}

} // namespace Antares::TSGenerator

#endif // __ANTARES_SOLVER_timeSeries_GENERATOR_HXX__
