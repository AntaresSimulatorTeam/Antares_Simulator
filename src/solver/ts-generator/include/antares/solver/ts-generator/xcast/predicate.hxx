// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__
#define __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__

#include <cmath>

#include <yuni/yuni.h>

#include <antares/series/series.h>
#include <antares/study/area/area.h>
#include "antares/study/parts/load/prepro.h"

namespace Antares::TSGenerator::Predicate
{
class Wind final
{
public:
    static bool preproDataIsReader(const Data::Area& area)
    {
        return area.wind.prepro != NULL;
    }

    static const char* timeSeriesName()
    {
        return "wind";
    }

    bool accept(const Data::Area& area) const
    {
        assert(area.wind.prepro != NULL);
        return !Utils::isZero(area.wind.prepro->xcast.capacity);
    }

    Data::TimeSeries::TS& matrix(Data::Area& area) const
    {
        return area.wind.series.timeSeries;
    }

    Data::XCast& xcastData(Data::Area& area) const
    {
        assert(area.wind.prepro != NULL);
        return area.wind.prepro->xcast;
    }

    static const Data::Correlation& correlation(const Data::Study& study)
    {
        return study.preproWindCorrelation;
    }

    uint timeSeriesToGenerate(const Data::Study& study) const
    {
        return study.parameters.nbTimeSeriesWind;
    }

}; // class Wind

class Load final
{
public:
    static bool preproDataIsReader(const Data::Area& area)
    {
        return area.load.prepro != NULL;
    }

    static const char* timeSeriesName()
    {
        return "load";
    }

    bool accept(const Data::Area& area) const
    {
        assert(area.load.prepro != NULL);
        return !Utils::isZero(area.load.prepro->xcast.capacity);
    }

    Data::TimeSeries::TS& matrix(Data::Area& area) const
    {
        return area.load.series.timeSeries;
    }

    Data::XCast& xcastData(Data::Area& area) const
    {
        assert(area.load.prepro != NULL);
        return area.load.prepro->xcast;
    }

    static const Data::Correlation& correlation(const Data::Study& study)
    {
        return study.preproLoadCorrelation;
    }

    uint timeSeriesToGenerate(const Data::Study& study) const
    {
        return study.parameters.nbTimeSeriesLoad;
    }

}; // class Load

class Solar final
{
public:
    static bool preproDataIsReader(const Data::Area& area)
    {
        return area.solar.prepro != NULL;
    }

    static const char* timeSeriesName()
    {
        return "solar";
    }

    bool accept(const Data::Area& area) const
    {
        assert(area.solar.prepro != NULL);
        return !Utils::isZero(area.solar.prepro->xcast.capacity);
    }

    Data::TimeSeries::TS& matrix(Data::Area& area) const
    {
        return area.solar.series.timeSeries;
    }

    Data::XCast& xcastData(Data::Area& area) const
    {
        assert(area.solar.prepro != NULL);
        return area.solar.prepro->xcast;
    }

    static const Data::Correlation& correlation(const Data::Study& study)
    {
        return study.preproSolarCorrelation;
    }

    uint timeSeriesToGenerate(const Data::Study& study) const
    {
        return study.parameters.nbTimeSeriesSolar;
    }

}; // class Solar

} // namespace Antares::TSGenerator::Predicate

#endif // __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__
