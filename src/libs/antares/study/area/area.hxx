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
#ifndef __ANTARES_LIBS_STUDY_AREAS_HXX__
#define __ANTARES_LIBS_STUDY_AREAS_HXX__

#include "../../utils.h"

namespace Antares
{
namespace Data
{
struct CompareAreaName final
{
    inline bool operator()(const Area* s1, const Area* s2) const
    {
        return s1->name < s2->name;
    }
};

template<int TimeSeriesT>
inline bool Area::storeTimeseriesNumbers(Study& study)
{
    switch (TimeSeriesT)
    {
    // Load
    case timeSeriesLoad:
        return storeTimeseriesNumbersForLoad(study);
        // Solar
    case timeSeriesSolar:
        return storeTimeseriesNumbersForSolar(study);
        // Hydro
    case timeSeriesHydro:
        return storeTimeseriesNumbersForHydro(study);
        // Wind
    case timeSeriesWind:
        return storeTimeseriesNumbersForWind(study);
        // Thermal
    case timeSeriesThermal:
        return storeTimeseriesNumbersForThermal(study);

        // Useless here
    case timeSeriesCount:
        break;
    }
    return false;
}

inline Area* AreaList::operator[](uint i)
{
    assert(i < areas.size() and "Index out of bounds");
    return byIndex[i];
}

inline const Area* AreaList::operator[](uint i) const
{
    assert(i < areas.size() and "Index out of bounds");
    return byIndex[i];
}

template<enum TimeSeries T>
inline XCast* Area::xcastData()
{
    switch (T)
    {
    case timeSeriesLoad:
        return &(load.prepro->xcast);
    case timeSeriesSolar:
        return &(solar.prepro->xcast);
    case timeSeriesWind:
        return &(wind.prepro->xcast);
    case timeSeriesHydro:
        return nullptr;
    case timeSeriesThermal:
        return nullptr;
    }
    return nullptr;
}

template<enum TimeSeries T>
inline const XCast* Area::xcastData() const
{
    switch (T)
    {
    case timeSeriesLoad:
        return &(load.prepro->xcast);
    case timeSeriesSolar:
        return &(solar.prepro->xcast);
    case timeSeriesWind:
        return &(wind.prepro->xcast);
    case timeSeriesHydro:
        return nullptr;
    case timeSeriesThermal:
        return nullptr;
    }
    return nullptr;
}

template<class PredicateT>
inline void AreaList::each(const PredicateT& predicate)
{
    auto end = areas.end();
    for (auto i = areas.begin(); i != end; ++i)
    {
        auto& area = *(i->second);
        predicate(area);
    }
}

template<class PredicateT>
inline void AreaList::each(const PredicateT& predicate) const
{
    auto end = areas.cend();
    for (auto i = areas.cbegin(); i != end; ++i)
    {
        auto& area = *(i->second);
        predicate(area);
    }
}

inline uint AreaList::size() const
{
    return (uint)areas.size();
}

inline AreaList::iterator AreaList::begin()
{
    return areas.begin();
}

inline AreaList::const_iterator AreaList::begin() const
{
    return areas.begin();
}

inline AreaList::const_iterator AreaList::cbegin() const
{
    return areas.begin();
}

inline AreaList::iterator AreaList::end()
{
    return areas.end();
}

inline AreaList::const_iterator AreaList::end() const
{
    return areas.end();
}

inline AreaList::const_iterator AreaList::cend() const
{
    return areas.end();
}

inline AreaList::reverse_iterator AreaList::rbegin()
{
    return areas.rbegin();
}

inline AreaList::const_reverse_iterator AreaList::rbegin() const
{
    return areas.rbegin();
}

inline AreaList::reverse_iterator AreaList::rend()
{
    return areas.rend();
}

inline AreaList::const_reverse_iterator AreaList::rend() const
{
    return areas.rend();
}

} // namespace Data
} // namespace Antares

#endif //  __ANTARES_LIBS_STUDY_AREAS_HXX__
