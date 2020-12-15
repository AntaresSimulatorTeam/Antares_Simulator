/*
** Copyright 2007-2018 RTE
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
#ifndef __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__
#define __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__

#include <yuni/yuni.h>
#include <antares/study/area.h>
#include <yuni/core/math.h>

namespace Antares
{
namespace Solver
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
        return !Yuni::Math::Zero(area.wind.prepro->xcast.capacity);
    }

    Matrix<double, Yuni::sint32>& matrix(Data::Area& area) const
    {
        assert(area.wind.series != NULL);
        return area.wind.series->series;
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
        return !Yuni::Math::Zero(area.load.prepro->xcast.capacity);
    }

    Matrix<double, Yuni::sint32>& matrix(Data::Area& area) const
    {
        assert(area.load.series != NULL);
        return area.load.series->series;
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
        return !Yuni::Math::Zero(area.solar.prepro->xcast.capacity);
    }

    Matrix<double, Yuni::sint32>& matrix(Data::Area& area) const
    {
        assert(area.solar.series != NULL);
        return area.solar.series->series;
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
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_TS_GENERATOR_XCAST_PREDICATE_HXX__
