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
#ifndef __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__
#define __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__

#include <cmath>

#include <yuni/yuni.h>

#include <antares/series/series.h>
#include <antares/study/area/area.h>
#include "antares/study/parts/load/prepro.h"

namespace Antares
{
namespace TSGenerator
{
namespace Predicate
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

} // namespace Predicate
} // namespace TSGenerator
} // namespace Antares

#endif // __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__
