// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_AREA_HXX__
#define __SOLVER_VARIABLE_AREA_HXX__

#include <antares/study/filter.h>
#include "antares/solver/variable/economy/dispatchable-generation-margin.h"

namespace Antares::Solver::Variable
{
template<class VariableList>
inline void Areas<VariableList>::initializeFromArea(Data::Study*, Data::Area*)
{
    // Nothing to do here
    // This method is called by initializeFromStudy() to all children
}

template<class VariableList>
inline void Areas<VariableList>::initializeFromLink(Data::Study*, Data::AreaLink*)
{
    // Nothing to do here
}

template<class VariableList>
void Areas<VariableList>::buildSurveyReport(SurveyResults& results,
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

template<class VariableList>
void Areas<VariableList>::buildAnnualSurveyReport(SurveyResults& results,
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

template<class VariableList>
void Areas<VariableList>::buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
{
    int count_int = count;
    if (count_int)
    {
        if (dataLevel & Category::DataLevel::area)
        {
            assert(pAreas.size() == results.data.study.areas.size());

            // Reset captions
            results.data.rowCaptions.clear();
            results.data.rowCaptions.resize(pAreas.size());

            // For each area
            for (uint i = 0; i != pAreas.size(); ++i)
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

template<class VariableList>
template<class PredicateT>
inline void Areas<VariableList>::RetrieveVariableList(PredicateT& predicate)
{
    VariableList::RetrieveVariableList(predicate);
}

template<class VariableList>
template<class SearchVCardT, class O>
inline void Areas<VariableList>::computeSpatialAggregateWith(O&)
{
    // Do nothing
}

template<class VariableList>
template<class SearchVCardT, class O>
inline void Areas<VariableList>::computeSpatialAggregateWith(O& out,
                                                             const Data::Area* area,
                                                             uint numSpace)
{
    assert(NULL != area);
    pAreas[area->index].template computeSpatialAggregateWith<SearchVCardT, O>(out, numSpace);
}

template<class VariableList>
template<class VCardToFindT>
inline void Areas<VariableList>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    pAreas[area->index].template retrieveResultsForArea<VCardToFindT>(result, area);
}

template<class VariableList>
template<class VCardToFindT>
inline void Areas<VariableList>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    pAreas[cluster->parentArea->index]
      .template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
}

template<class VariableList>
template<class VCardToFindT>
inline void Areas<VariableList>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    pAreas[link->from->index].template retrieveResultsForLink<VCardToFindT>(result, link);
}

template<class VariableList>
void Areas<VariableList>::initializeFromStudy(Data::Study& study)
{
    const uint pAreaCount = study.areas.size();

    pAreas.resize(pAreaCount);

    uint tick = 6;
    uint oldPercent = 0;
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        auto* currentArea = study.areas.byIndex[i];
        if (!(--tick))
        {
            uint newPercent = ((i * 100u) / pAreaCount);
            if (newPercent != oldPercent)
            {
                logs.info() << "Allocating resources " << ((i * 100u) / pAreaCount) << "%";
                oldPercent = newPercent;
            }
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

template<class VariableList>
void Areas<VariableList>::simulationBegin()
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].simulationBegin();
    }
}

template<class VariableList>
void Areas<VariableList>::simulationEnd()
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].simulationEnd();
    }
}

template<class VariableList>
void Areas<VariableList>::hourForEachArea(State& state, uint numSpace)
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

          for (std::size_t i = 0; i < area.shortTermStorage.storagesByIndex.size(); i++)
          {
              // Intiializing the state for the current short term storage cluster
              state.initFromShortTermStorageClusterIndex(i);
          }

          // Ajout de l'initialisation pour l'hydro
          state.initFromHydro();

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

template<class VariableList>
void Areas<VariableList>::weekForEachArea(State& state, uint numSpace)
{
    // For each area...
    state.study.areas.each(
      [this, &state, &numSpace](Data::Area& area)
      {
          state.area = &area; // the current area

          // Initializing the state for the current area
          state.initFromAreaIndex(area.index, numSpace);

          auto& variablesForArea = pAreas[area.index];

          variablesForArea.weekForEachArea(state, numSpace);

          // NOTE
          // currently, the event is not broadcasted to thermal
          // clusters and links
      }); // for each area
}

template<class VariableList>
void Areas<VariableList>::yearBegin(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].yearBegin(year, numSpace);
    }
}

template<class VariableList>
void Areas<VariableList>::buildThermalClusterYearEndResults(State& state, uint year, uint numSpace)
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

          // Calculation of reserve participation costs
          if (state.study.parameters.reservesEnabled)
          {
              state.calculateReserveParticipationCosts();
          }
      }); // for each area
}

template<class VariableList>
void Areas<VariableList>::yearEnd(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        // Broadcast to all areas
        pAreas[i].yearEnd(year, numSpace);
    }
}

template<class VariableList>
void Areas<VariableList>::computeSummary(unsigned int year, unsigned int numSpace)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        // Broadcast to all areas
        pAreas[i].computeSummary(year, numSpace);
    }
}

template<class VariableList>
void Areas<VariableList>::weekBegin(State& state)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].weekBegin(state);
    }
}

template<class VariableList>
void Areas<VariableList>::weekEnd(State& state)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].weekEnd(state);
    }
}

template<class VariableList>
void Areas<VariableList>::hourBegin(uint hourInTheYear)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].hourBegin(hourInTheYear);
    }
}

template<class VariableList>
void Areas<VariableList>::hourForEachLink(State& state, uint numSpace)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].hourForEachLink(state, numSpace);
    }
}

template<class VariableList>
void Areas<VariableList>::hourEnd(State& state, uint hourInTheYear)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].hourEnd(state, hourInTheYear);
    }
}

template<class VariableList>
void Areas<VariableList>::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreas.size(); ++i)
    {
        pAreas[i].beforeYearByYearExport(year, numSpace);
    }
}
} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_AREA_HXX__
