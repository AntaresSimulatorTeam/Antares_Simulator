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
#ifndef __SOLVER_VARIABLE_AREA_HXX__
#define __SOLVER_VARIABLE_AREA_HXX__

#include <antares/study/filter.h>

namespace Antares
{
namespace Solver
{
namespace Variable
{
template<class NextT>
inline Areas<NextT>::Areas()
{
    // Do nothing
}

template<class NextT>
inline void Areas<NextT>::initializeFromArea(Data::Study*, Data::Area*)
{
    // Nothing to do here
    // This method is called by initializeFromStudy() to all children
}

template<class NextT>
inline void Areas<NextT>::initializeFromAreaLink(Data::Study*, Data::AreaLink*)
{
    // Nothing to do here
}

template<class NextT>
inline void Areas<NextT>::initializeFromThermalCluster(Data::Study*,
                                                       Data::Area*,
                                                       Data::ThermalCluster*)
{
    // This method should not be called at this point
}

template<class NextT>
void Areas<NextT>::buildSurveyReport(SurveyResults& results,
                                     int dataLevel,
                                     int fileLevel,
                                     int precision) const
{
    int count_int = count;
    bool linkDataLevel = dataLevel & Category::link;
    bool areaDataLevel = dataLevel & Category::area;
    bool thermalAggregateDataLevel = dataLevel & Category::thermalAggregate;
    if (count_int && (linkDataLevel || areaDataLevel || thermalAggregateDataLevel))
    {
        assert(results.data.area != NULL
               && "The area must not be null to generate a survey report");

        // Build the survey results for the given area
        auto& area = *results.data.area;

        // Filtering
        if (0 == (dataLevel & Category::link)) // filter on all but links
        {
            switch (precision)
            {
            case Category::hourly:
                if (not(area.filterSynthesis & Data::filterHourly))
                    return;
                break;
            case Category::daily:
                if (not(area.filterSynthesis & Data::filterDaily))
                    return;
                break;
            case Category::weekly:
                if (not(area.filterSynthesis & Data::filterWeekly))
                    return;
                break;
            case Category::monthly:
                if (not(area.filterSynthesis & Data::filterMonthly))
                    return;
                break;
            case Category::annual:
                if (not(area.filterSynthesis & Data::filterAnnual))
                    return;
                break;
            case Category::all:
                break;
            }
        }
        pAreas[area.index].buildSurveyReport(results, dataLevel, fileLevel, precision);
    }
}

template<class NextT>
void Areas<NextT>::buildAnnualSurveyReport(SurveyResults& results,
                                           int dataLevel,
                                           int fileLevel,
                                           int precision,
                                           uint numSpace) const
{
    int count_int = count;
    bool linkDataLevel = dataLevel & Category::link;
    bool areaDataLevel = dataLevel & Category::area;
    bool thermalAggregateDataLevel = dataLevel & Category::thermalAggregate;
    if (count_int && (linkDataLevel || areaDataLevel || thermalAggregateDataLevel))
    {
        assert(results.data.area != NULL
               && "The area must not be null to generate a survey report");

        auto& area = *results.data.area;

        // Filtering
        if (0 == (dataLevel & Category::link)) // filter on all but links
        {
            switch (precision)
            {
            case Category::hourly:
                if (!(area.filterYearByYear & Data::filterHourly))
                    return;
                break;
            case Category::daily:
                if (!(area.filterYearByYear & Data::filterDaily))
                    return;
                break;
            case Category::weekly:
                if (!(area.filterYearByYear & Data::filterWeekly))
                    return;
                break;
            case Category::monthly:
                if (!(area.filterYearByYear & Data::filterMonthly))
                    return;
                break;
            case Category::annual:
                if (!(area.filterYearByYear & Data::filterAnnual))
                    return;
                break;
            case Category::all:
                break;
            }
        }

        // Build the survey results for the given area
        pAreas[area.index].buildAnnualSurveyReport(
          results, dataLevel, fileLevel, precision, numSpace);
    }
}

template<class NextT>
template<class PredicateT>
inline void Areas<NextT>::RetrieveVariableList(PredicateT& predicate)
{
    NextType::RetrieveVariableList(predicate);
}

template<class NextT>
template<class I>
inline void Areas<NextT>::provideInformations(I& infos)
{
    // Begining of the node
    if (VCardType::nodeDepthForGUI)
    {
        infos.template beginNode<VCardType>();
        // Next variable in the list
        NextType::template provideInformations<I>(infos);
        // End of the node
        infos.endNode();
    }
    else
    {
        // Giving our VCard
        infos.template addVCard<VCardType>();
        // Next variable in the list
        NextType::template provideInformations<I>(infos);
    }
}

template<class NextT>
template<class SearchVCardT, class O>
inline void Areas<NextT>::computeSpatialAggregateWith(O&)
{
    // Do nothing
}

template<class NextT>
template<class SearchVCardT, class O>
inline void Areas<NextT>::computeSpatialAggregateWith(O& out, const Data::Area* area, uint numSpace)
{
    assert(NULL != area);
    pAreas[area->index].template computeSpatialAggregateWith<SearchVCardT, O>(out, numSpace);
}

template<class NextT>
template<class VCardToFindT>
const double* Areas<NextT>::retrieveHourlyResultsForCurrentYear() const
{
    return nullptr;
}

template<class NextT>
template<class VCardToFindT>
inline void Areas<NextT>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    pAreas[area->index].template retrieveResultsForArea<VCardToFindT>(result, area);
}

template<class NextT>
template<class VCardToFindT>
inline void Areas<NextT>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    pAreas[cluster->parentArea->index].template retrieveResultsForThermalCluster<VCardToFindT>(
      result, cluster);
}

template<class NextT>
template<class VCardToFindT>
inline void Areas<NextT>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    pAreas[link->from->index].template retrieveResultsForLink<VCardToFindT>(result, link);
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_AREA_HXX__
