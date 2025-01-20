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
#ifndef __SOLVER_VARIABLE_AREA_HXX__
#define __SOLVER_VARIABLE_AREA_HXX__

#include <antares/study/filter.h>
#include "antares/solver/variable/economy/dispatchable-generation-margin.h"

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
    bool linkDataLevel = dataLevel & Category::DataLevel::link;
    bool areaDataLevel = dataLevel & Category::DataLevel::area;
    bool thermalAggregateDataLevel = dataLevel & Category::DataLevel::thermalAggregate;
    if (count_int && (linkDataLevel || areaDataLevel || thermalAggregateDataLevel))
    {
        assert(results.data.area != NULL
               && "The area must not be null to generate a survey report");

        // Build the survey results for the given area
        auto& area = *results.data.area;

        // Filtering
        if (0 == (dataLevel & Category::DataLevel::link)) // filter on all but links
        {
            switch (precision)
            {
            case Category::hourly:
                if (not(area.filterSynthesis & Data::filterHourly))
                {
                    return;
                }
                break;
            case Category::daily:
                if (not(area.filterSynthesis & Data::filterDaily))
                {
                    return;
                }
                break;
            case Category::weekly:
                if (not(area.filterSynthesis & Data::filterWeekly))
                {
                    return;
                }
                break;
            case Category::monthly:
                if (not(area.filterSynthesis & Data::filterMonthly))
                {
                    return;
                }
                break;
            case Category::annual:
                if (not(area.filterSynthesis & Data::filterAnnual))
                {
                    return;
                }
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
    bool linkDataLevel = dataLevel & Category::DataLevel::link;
    bool areaDataLevel = dataLevel & Category::DataLevel::area;
    bool thermalAggregateDataLevel = dataLevel & Category::DataLevel::thermalAggregate;
    if (count_int && (linkDataLevel || areaDataLevel || thermalAggregateDataLevel))
    {
        assert(results.data.area != NULL
               && "The area must not be null to generate a survey report");

        auto& area = *results.data.area;

        // Filtering
        if (0 == (dataLevel & Category::DataLevel::link)) // filter on all but links
        {
            switch (precision)
            {
            case Category::hourly:
                if (!(area.filterYearByYear & Data::filterHourly))
                {
                    return;
                }
                break;
            case Category::daily:
                if (!(area.filterYearByYear & Data::filterDaily))
                {
                    return;
                }
                break;
            case Category::weekly:
                if (!(area.filterYearByYear & Data::filterWeekly))
                {
                    return;
                }
                break;
            case Category::monthly:
                if (!(area.filterYearByYear & Data::filterMonthly))
                {
                    return;
                }
                break;
            case Category::annual:
                if (!(area.filterYearByYear & Data::filterAnnual))
                {
                    return;
                }
                break;
            case Category::all:
                break;
            }
        }

        // Build the survey results for the given area
        pAreas[area.index].buildAnnualSurveyReport(results,
                                                   dataLevel,
                                                   fileLevel,
                                                   precision,
                                                   numSpace);
    }
}

template<class NextT>
void Areas<NextT>::buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
{
    int count_int = count;
    if (count_int)
    {
        if (dataLevel & Category::DataLevel::area)
        {
            assert(pAreaCount == results.data.study.areas.size());

            // Reset captions
            results.data.rowCaptions.clear();
            results.data.rowCaptions.resize(pAreaCount);

            // For each area
            // for (uint i = 0; i != results.data.study.areas.byIndex.size(); ++i)
            for (uint i = 0; i != pAreaCount; ++i)
            {
                results.data.area = results.data.study.areas[i];
                uint index = results.data.area->index;
                results.data.rowIndex = index;
                results.data.rowCaptions[index] = results.data.area->id;
                results.data.columnIndex = 0;
                results.resetValuesAtLine(i);
                pAreas[i].buildDigest(results, digestLevel, dataLevel);
            }
        }
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
    pAreas[cluster->parentArea->index]
      .template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
}

template<class NextT>
template<class VCardToFindT>
inline void Areas<NextT>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    pAreas[link->from->index].template retrieveResultsForLink<VCardToFindT>(result, link);
}

template<class NextT>
Areas<NextT>::~Areas()
{
    // Releasing the memory occupied by the areas
    delete[] pAreas;
}

template<class NextT>
void Areas<NextT>::initializeFromStudy(Data::Study& study)
{
    // The total number of areas
    pAreaCount = study.areas.size();

    // Reserving the memory
    pAreas = new NextType[pAreaCount];

    // For each area...
    uint tick = 6;
    uint oldPercent = 0;
    for (uint i = 0; i != pAreaCount; ++i)
    {
        // Instancing a new set of variables of the area
        auto* currentArea = study.areas.byIndex[i];
        if (!(--tick))
        {
            uint newPercent = ((i * 100u) / pAreaCount);
            if (newPercent != oldPercent)
            {
                logs.info() << "Allocating resources " << ((i * 100u) / pAreaCount) << "%";
                oldPercent = newPercent;
            }
            // Reset the tick
            tick = 6;
        }

        // Initialize the variables
        // From the study
        pAreas[i].initializeFromStudy(study);
        // From the area
        pAreas[i].initializeFromArea(&study, currentArea);
        // Does current output variable appears non applicable in areas' output files, not
        // districts'. Note that digest gather area and district results.
        pAreas[i].broadcastNonApplicability(not currentArea->hydro.reservoirManagement);

        // For each current area's variable, getting the print status, that is :
        // is variable's column(s) printed in output (areas) reports ?
        pAreas[i].getPrintStatusFromStudy(study);

        pAreas[i].supplyMaxNumberOfColumns(study);
    }
}

template<class NextT>
void Areas<NextT>::simulationBegin()
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].simulationBegin();
    }
}

template<class NextT>
void Areas<NextT>::simulationEnd()
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].simulationEnd();
    }
}

template<class NextT>
void Areas<NextT>::hourForEachArea(State& state, uint numSpace)
{
    // For each area...
    state.study.areas.each(
      [this, &state, &numSpace](Data::Area& area)
      {
          state.area = &area; // the current area

          // Initializing the state for the current area
          state.initFromAreaIndex(area.index, numSpace);

          for (const auto& cluster: area.thermal.list.each_enabled())
          {
              // Intiializing the state for the current thermal cluster
              state.initFromThermalClusterIndex(cluster->enabledIndex);
          }

          // Variables
          auto& variablesForArea = pAreas[area.index];
          variablesForArea.hourForEachArea(state, numSpace);

          // All links
          auto end = area.links.end();
          for (auto i = area.links.begin(); i != end; ++i)
          {
              state.link = i->second;
              // Variables
              variablesForArea.hourForEachLink(state, numSpace);
          }
      }); // for each area
}

template<class NextT>
void Areas<NextT>::weekForEachArea(State& state, uint numSpace)
{
    // For each area...
    state.study.areas.each(
      [this, &state, &numSpace](Data::Area& area)
      {
          state.area = &area; // the current area

          // Initializing the state for the current area
          state.initFromAreaIndex(area.index, numSpace);

          auto& variablesForArea = pAreas[area.index];

          // DTG MRG
          state.dispatchableMargin = variablesForArea.template retrieveHourlyResultsForCurrentYear<
            Economy::VCardDispatchableGenMargin>(numSpace);

          variablesForArea.weekForEachArea(state, numSpace);

          // NOTE
          // currently, the event is not broadcasted to thermal
          // clusters and links
      }); // for each area
}

template<class NextT>
void Areas<NextT>::yearBegin(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].yearBegin(year, numSpace);
    }
}

template<class NextT>
void Areas<NextT>::yearEndBuild(State& state, uint year, uint numSpace)
{
    // For each area...
    state.study.areas.each(
      [this, &state, &year, &numSpace](Data::Area& area)
      {
          state.area = &area; // the current area

          // Initializing the state for the current area
          state.initFromAreaIndex(area.index, numSpace);

          // Variables
          auto& variablesForArea = pAreas[area.index];

          for (const auto& cluster: area.thermal.list.each_enabled())
          {
              state.thermalCluster = cluster.get();
              state.yearEndResetThermal();

              // Variables
              variablesForArea.yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);

              // Building the end of year
              state.yearEndBuildFromThermalClusterIndex(cluster->enabledIndex);

              // Variables
              variablesForArea.yearEndBuildForEachThermalCluster(state, year, numSpace);
          } // for each thermal cluster
      }); // for each area
}

template<class NextT>
void Areas<NextT>::yearEnd(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        // Broadcast to all areas
        pAreas[i].yearEnd(year, numSpace);
    }
}

template<class NextT>
void Areas<NextT>::computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                                  unsigned int nbYearsForCurrentSummary)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        // Broadcast to all areas
        pAreas[i].computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }
}

template<class NextT>
void Areas<NextT>::weekBegin(State& state)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].weekBegin(state);
    }
}

template<class NextT>
void Areas<NextT>::weekEnd(State& state)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].weekEnd(state);
    }
}

template<class NextT>
void Areas<NextT>::hourBegin(uint hourInTheYear)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].hourBegin(hourInTheYear);
    }
}

template<class NextT>
void Areas<NextT>::hourForEachLink(State& state, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].hourForEachLink(state, numSpace);
    }
}

template<class NextT>
void Areas<NextT>::hourEnd(State& state, uint hourInTheYear)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].hourEnd(state, hourInTheYear);
    }
}

template<class NextT>
void Areas<NextT>::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].beforeYearByYearExport(year, numSpace);
    }
}
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_AREA_HXX__
